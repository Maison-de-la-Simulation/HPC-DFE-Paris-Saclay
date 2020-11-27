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
    use mpi
    
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

        if (rank == 0) then
        
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

            if (rank == 0) then

                print('(X,"  ",I8," | ",E15.6, "  | ")'),&
                step,grid_integration
            
            end if

            
        end if
        
    end subroutine
    
    ! __________________________________________________________________________
    !
    ! FR: Calcule l'intégrale de la grille
    ! ENG: Compute the integration of the grid
    ! __________________________________________________________________________
    subroutine compute_grid_integration(integration)
        implicit none

        integer              :: i,j,ierror
        real(8), intent(out) :: integration
        real(8) :: local_integration
        real(8)              :: total_integration

        local_integration = 0.                    ! ! Variable d'intégration globale

        ! Boucle sur les indices y
        do j=2,local_Ny-1
            ! Boucle sur indices en x
            do i=2,local_Nx - 1
                local_integration = local_integration + curr_grid(i,j)**2*dx**2
            enddo
        enddo

        ! MPI Reduction
        CALL MPI_REDUCE(local_integration, total_integration, 1, &
                        MPI_DOUBLE_PRECISION, MPI_SUM, 0, cartesian_communicator,ierror)
                        
        integration = total_integration

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR: Calcule l'intégrale de la grille
    ! ENG: Compute the integration of the grid
    ! __________________________________________________________________________
    subroutine output_grid(curr_grid, step, output_period, local_Nx, local_Ny, Nx, Ny, Lx, Ly)
        implicit none

        integer, intent(in) :: step
        integer, intent(in) :: output_period
        integer, intent(in) :: Nx, Ny            ! Nombre de points dans chaque direction pour tout le domaine
        integer, intent(in) :: local_Nx, local_Ny
        real(8), intent(in) :: Lx, Ly            ! Taille du domaine dans chaque dimension
        character(len=64)   :: filename          ! File name
        
        real(8), dimension(:,:), allocatable, intent(in) :: curr_grid ! Le tableau de la grille structurée au temps courant
        
        real(8), dimension(Nx,Ny) :: global_grid
        real(8), dimension(local_Nx-2,local_Ny-2) :: buffer_grid
        
        integer             :: iy, ierror, imin, imax, jmin, jmax, tag, i
        
        integer, dimension(2)   :: rc

        if (mod(step,output_period) .eq. 0) then

            do i=1,number_of_ranks-1
                
                if (rank == i ) then
                    
                    
                    Call MPI_SEND(curr_grid(2:local_Nx-1,2:local_Ny-1), & ! La partie du tableau à envoyer
                                      (local_Ny-2)*(local_Nx-2),  & ! Le nombre d'élèments
                                      MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                                      0,          &
                                      i, &
                                      cartesian_communicator,     & ! Le communicateur
                                      ierror)                       ! Code d'erreur
                endif
                
                if ( rank == 0) then
                    CALL MPI_CART_COORDS(cartesian_communicator, &    ! Communicateur cartésien
                                     i, &                       ! Rang du processus courant
                                     2, &                          ! Dimension
                                     rc, &           ! Coordonnées retournées du processus
                                     ierror)                     ! Code d'erreur
                                     
                                     
                    imin = rc(1)*(local_Nx-2)+1
                    imax = (rc(1)+1)*(local_Nx-2)
                    
                    jmin = rc(2)*(local_Ny-2)+1
                    jmax = (rc(2)+1)*(local_Ny-2)
                                     
                    ! global_grid(imin:imax,jmin:jmax)
                                     
                    Call MPI_Recv(global_grid(imin:imax,jmin:jmax), &
                                (local_Ny-2)*(local_Nx-2), &
                                MPI_DOUBLE_PRECISION, &
                                i, i, &
                                cartesian_communicator, MPI_STATUS_IGNORE, ierror)
                                     
                endif

                
            enddo

            if (rank == 0) then

                global_grid(1:local_Nx-2,1:local_Ny-2) = curr_grid(2:local_Nx-1,2:local_Ny-1)

                write(filename,'("diags/grid_",I5.5,".dat")') step

                open(10, file=filename, status='new' ,access='stream')
                
                write(10) Nx, Ny, Lx, Ly
                    
                write(10) global_grid(:,:)
                
                close(unit=10)
            
            end if
        
        end if

    end subroutine

end module
