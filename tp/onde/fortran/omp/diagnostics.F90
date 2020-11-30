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


        if (mod(step,print_period) .eq. 0) then

            Call compute_grid_integration(grid_integration)

            print('(X,"  ",I8," | ",E15.6, "  | ")'),&
            step,grid_integration

            
        end if

    end subroutine
    
    ! __________________________________________________________________________
    !
    ! FR: Calcule l'intégrale de la grille
    ! ENG: Compute the integration of the grid
    ! __________________________________________________________________________
    subroutine compute_grid_integration(integration)
        implicit none

        integer              :: i,j
        real(8), intent(out) :: integration

        integration = 0.                    ! ! Variable d'intégration globale

        ! Boucle sur les indices y
        do j=2,Ny-1
            ! Boucle sur indices en x
            do i=2,Nx - 1
                integration = integration + curr_grid(i,j)**2*dx**2
            enddo
        enddo

        ! MPI Reduction
        ! ....

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR: Calcule l'intégrale de la grille
    ! ENG: Compute the integration of the grid
    ! __________________________________________________________________________
    subroutine output_grid(curr_grid, step, output_period, Nx, Ny, Lx, Ly)
        implicit none

        integer, intent(in) :: step
        integer, intent(in) :: output_period
        integer, intent(in) :: Nx, Ny            ! Nombre de points dans chaque direction pour tout le domaine
        real(8), intent(in) :: Lx, Ly            ! Taille du domaine dans chaque dimension
        character(len=64)   :: filename          ! File name
        
        real(8), dimension(:,:), allocatable, intent(in) :: curr_grid ! Le tableau de la grille structurée au temps courant
        
        integer             :: iy

        if (mod(step,output_period) .eq. 0) then

            ! MPI communication
            ! ....

            write(filename,'("diags/grid_",I5.5,".dat")') step

            open(10, file=filename, status='new' ,access='stream')
            
            write(10) Nx, Ny, Lx, Ly
                
            write(10) curr_grid
            
            close(unit=10)
        
        end if

    end subroutine

end module
