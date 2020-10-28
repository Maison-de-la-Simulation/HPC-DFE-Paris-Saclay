! ______________________________________________________________________________
!
! Module diagnostics
!
! Description: Ce module contient les fonctions d'IO. Cela inclue la grille de manière
! parallèle et les grandeurs totales.
!
! ______________________________________________________________________________
module diagnostics

    use parameters
    use parallelism
    use hdf5
    implicit none

    contains

    ! __________________________________________________________________________
    !
    ! FR: imprime à l'écran les informations de l'itération courante
    ! ENG: Print useful information for the current time step
    ! __________________________________________________________________________
    subroutine print_timestep_information(step)
        implicit none

        integer, intent(in) :: step
        real(8)             :: grid_integration

        if (rank.eq.0) then
            if (first_print) then
                print*, ' '
                print('(X,"_______________________________")')
                print('(X)')
                print('(X,"        Time loop              ")')
                print('(X,"_______________________________")')
                print('(X,"           |                  |")')
                print('(X,"  Timestep | grid integration |")')
                print('(X," __________|__________________|")')
                first_print = .false.
            end if
        end if

        if (mod(step,print_period) .eq. 0) then

            Call compute_grid_integration(grid_integration)

            if (rank.eq.0) print('(X,"  ",I8," | ",E15.6, "  | ")'),&
            step,grid_integration

        end if

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR: Ecriture des diagnostiques
    ! ENG: Output the diagnostics
    ! __________________________________________________________________________
    subroutine output_diags(step)
        implicit none

        integer, intent(in) :: step          ! Current time step (used for the file name)

        if (mod(step,diagnostic_period) .eq. 0) then
            call output_grid(step)
        endif

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR:
    ! Ecriture de la grille en parallèle en hdf5
    ! ENG:
    ! Output the grid in parallel hdf5
    ! __________________________________________________________________________
    subroutine output_grid(step)
        implicit none

        integer, intent(in) :: step          ! Current time step (used for the file name)
        integer             :: ierror
        integer             :: info
        integer(HID_T)      :: plist_id      ! Property list identifier
        integer(HID_T)      :: file_id       ! File identifier
        integer(HID_T)      :: dset_id       ! Dataset identifier
        integer(HID_T)      :: filespace     ! Dataspace identifier in file
        integer(HID_T)      :: memspace      ! Dataspace identifier in memory
        integer             :: dataset_rank = 2      ! Dataset rank

        integer(HSIZE_T), dimension(2) :: count
        integer(HSIZE_T), dimension(2) :: offset
        integer(HSIZE_T), dimension(2) :: stride
        integer(HSIZE_T), dimension(2) :: block

        integer(HSIZE_T), dimension(2) :: dataset_dims ! Dataset dimensions in the file.
        integer(HSIZE_T), dimension(2) :: chunk_dims   ! Chunks dimensions

        ! For attributes
        integer(HID_T)      :: attr_id
        integer(HID_T)      :: attr_space_id

        character(len=64)   :: filename          ! File name
        character(len=64)   :: dataset_name      ! Dataset name

        info = MPI_INFO_NULL

        dataset_dims = (/ Nx , Ny /)
        chunk_dims   = (/ local_Nx - 2 , local_Ny - 2 /)

        ! Create the filename
        write(filename,'("diags/grid_",I5.5,".h5")') step
        write(dataset_name,'("data")')

        !write(filename,'("charge.h5")')
        !write(dataset_name,'("charge_",I3.3)') step

        ! Initialize HDF5 library and Fortran interfaces.
        CALL h5open_f(ierror)

        ! Setup file access property list with parallel I/O access.
        CALL h5pcreate_f(H5P_FILE_ACCESS_F, plist_id, ierror)
        CALL h5pset_fapl_mpio_f(plist_id, cartesian_communicator, info, ierror)

        ! Create the file collectively.
        !CALL h5fcreate_f(trim(filename), H5F_ACC_RDWR_F, file_id, ierror, access_prp = plist_id)
        CALL h5fcreate_f(trim(filename), H5F_ACC_TRUNC_F, file_id, ierror, access_prp = plist_id)
        CALL h5pclose_f(plist_id, ierror)

        ! Create the data space for the dataset.
        CALL h5screate_simple_f(dataset_rank, dataset_dims, filespace, ierror)
        CALL h5screate_simple_f(dataset_rank, chunk_dims, memspace, ierror)

        ! Create chunked dataset.
        CALL h5pcreate_f(H5P_DATASET_CREATE_F, plist_id, ierror)
        CALL h5pset_chunk_f(plist_id, dataset_rank, chunk_dims, ierror)
        CALL h5dcreate_f(file_id, dataset_name, H5T_NATIVE_DOUBLE, filespace, &
                          dset_id, ierror, plist_id)
        CALL h5sclose_f(filespace, ierror)

        ! Each process defines dataset in memory and writes it to the hyperslab
        ! in the file.
        stride(1) = 1
        stride(2) = 1
        count(1) =  1
        count(2) =  1
        ! Taille du bloc
        block(1) = chunk_dims(1)
        block(2) = chunk_dims(2)
        ! Position du bloc en nombre de points dans la grille finale
        offset(1) = rank_coordinates(1)*chunk_dims(1)
        offset(2) = rank_coordinates(2)*chunk_dims(2)

        ! Select hyperslab in the file.
        CALL h5dget_space_f(dset_id, filespace, ierror)
        CALL h5sselect_hyperslab_f(filespace, H5S_SELECT_SET_F, offset, count, ierror, &
                                 stride, block)

        ! Create property list for collective dataset write
        CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, ierror)
        CALL h5pset_dxpl_mpio_f(plist_id, H5FD_MPIO_COLLECTIVE_F, ierror)

        ! Write the dataset collectively.
        CALL h5dwrite_f(dset_id, H5T_NATIVE_DOUBLE, &
                        (U(2:local_Nx-1,2:local_Ny-1)), &
                        dataset_dims, ierror, file_space_id = filespace, &
                        mem_space_id = memspace, xfer_prp = plist_id)

        ! Attributes - this does not properly in Fortran
        ! Call H5Screate_f(H5S_SCALAR_F, attr_space_id, ierror);
        ! Call H5Acreate2_f(dset_id, "xdelta", H5T_NATIVE_DOUBLE, attr_space_id, &
        !                  attr_id, ierror, H5P_DEFAULT_F, H5P_DEFAULT_F)
        ! Call H5Awrite(attr_id, H5T_NATIVE_DOUBLE, subdomain%delta(1), ierror)

        ! Close dataspaces.
        CALL h5sclose_f(filespace, ierror)
        CALL h5sclose_f(memspace, ierror)

        ! Close the dataset.
        CALL h5dclose_f(dset_id, ierror)

        ! Close the property list.
        CALL h5pclose_f(plist_id, ierror)

        ! Close the file.
        CALL h5fclose_f(file_id, ierror)

        ! Close FORTRAN interfaces and HDF5 library.
        CALL h5close_f(ierror)

    end subroutine

end module
