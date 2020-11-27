module physics
    
    implicit none

contains

    ! __________________________________________________________________________
    !
    ! Initialisation du domaine
    ! __________________________________________________________________________
    subroutine initialize_domain(Nx, Ny,                    &
                                 Lx, Ly,                    &
                                 alpha,                     &
                                 nt,                        &
                                 dt,                        &
                                 dx,                        &
                                 C,                         &
                                 coef,                      &
                                 prev_grid, curr_grid, next_grid)

        implicit none

        integer, intent(in)                  :: Nx, Ny                 ! Nombres de noeuds/points de la grille dans chaque direction
        real(8), intent(inout)               :: Lx, Ly                 ! Taille du domaine dans chaque dimension
        real(8), intent(in)                  :: alpha                  ! Facteur sur le pas de temps pour la condition de stabilité (< 1)
        integer, intent(in)                  :: Nt                     ! Nombre d'itération
        real(8), intent(in)                  :: C                      ! Vitesse du son
        real(8), intent(out)                 :: coef                   ! Coef pour le schéma
        real(8), dimension(:,:), allocatable, intent(out) :: prev_grid ! Tableau représentant la grille structurée au temps précédent
        real(8), dimension(:,:), allocatable, intent(out) :: curr_grid ! Le tableau de la grille structurée au temps courant
        real(8), dimension(:,:), allocatable, intent(out) :: next_grid ! Le tableau de la grille structurée au temps suivant
        real(8), intent(out)                 :: dt                     ! Pas de temps
        real(8), intent(out)                 :: dx                     ! Pas d'espace

        real(8) :: r
        integer :: i,j
        integer :: ierror

        ! Création du dossier de diagnostique
        Call system("rm -r diags")
        Call system("mkdir -p diags")
        
        ! Calcul des dimensions à partir des pas et du nombre de points
        Lx = Nx * dx
        Ly = Ny * dx

        ! Calcul du nombre de noeuds dans chaque dimension pour chaque rang MPI
        ! On rajoute deux noeuds pour gérer les cellules fantômes et stocker les noeuds voisins
        ! .....

        ! Taille des sous-domaine MPI
        ! .....

        ! Calcul des minimums locaux
        ! .....

        ! Calcul des maximums locaux
        ! .....

        ! Calcul du pas de temps avec respect de la condition de stabilité
        dt = alpha * dx / C

        ! Coefficient pour le schéma
        coef = (c * dt / dx)**2

        ! Allocation des grilles, il faut se rappeler que la première
        ! dimension est celle contiguë en mémoire
        allocate(prev_grid(Nx,Ny))
        allocate(curr_grid(Nx,Ny))
        allocate(next_grid(Nx,Ny))

        ! Initialisation des grilles
        prev_grid = 0
        curr_grid = 0
        next_grid = 0

        ! Condition intiale
        Do j=2,Ny-1
            Do i=2,Nx-1
                r = sqrt((i*dx - 0.5*Lx)**2 + (j*dx - 0.5*Ly)**2)
                curr_grid(i,j) = exp(-(r - 0.25*Lx)**2 / (0.1*Lx)**2 )
            End do
        Enddo

        ! Bilan des paramètres de simulation
        print*, ' '
        print('(X,"______________________________________________________________________________")')
        print('(X)')
        print('(X,"        Code equation d''onde                                                 ")')
        print('(X,"______________________________________________________________________________")')
        print('(X)')
        print('(X,"dx: ",(F8.3,X))'),dx
        print('(X,"C: ",(F8.3,X))'),C
        print('(X,"Lx: ",(F8.3,X),"Ly: ",(F8.3,X))'),Lx, Ly
        print('(X,"Nx: ",(I8,X),"Ny: ",(I8,X))'),Nx, Ny
        print('(X,"dt: ",(F8.3,X))'),dt
        print('(X,"Nt: ",(I8,X))'),Nt
        print('(X)')
        

    end subroutine

    ! __________________________________________________________________________
    !
    ! Calcul de l'équation d'onde au pas de temps suivant
    ! __________________________________________________________________________
    subroutine update_domain(prev_grid, curr_grid, next_grid, Nx, Ny, coef, dx, dt, Nt, omega, amplitude,  it)
        implicit none

        ! Coef pour le schéma
        real(8), intent(in)                                 :: coef
        ! Tableau représentant la grille structurée au temps suivant
        real(8), dimension(:,:), allocatable, intent(inout) :: next_grid
        ! Tableau représentant la grille structurée
        real(8), dimension(:,:), allocatable, intent(inout) :: curr_grid
        ! Le tableau de la grille structurée au temps précédent
        real(8), dimension(:,:), allocatable, intent(inout) :: prev_grid
        ! Nombres de noeuds/points de la grille dans chaque direction
        integer, intent(in)                                 :: Nx, Ny
        ! Pas de temps
        real(8), intent(in)                                 :: dt
        ! Nombre de pas de temps
        integer, intent(in)                                 :: Nt
        ! Iteration en cours
        integer, intent(in)                                 :: it
        ! Pas d'espace
        real(8), intent(in)                                 :: dx
        ! fréquence de la perturbation
        real(8), intent(in)                                 :: omega
        ! amplitude de la perturbation
        real(8), intent(in)                                 :: amplitude
        
        ! Source
        real(8)                                             :: source

        integer :: i,j

        ! Schéma différence finie explicite
        do j = 2,Ny-1
            do i = 2,Nx-1

                next_grid(i,j) = 2*curr_grid(i,j) - prev_grid(i,j) &
                               + coef * (curr_grid(i+1,j) + curr_grid(i-1,j) - 4*curr_grid(i,j) &
                               + curr_grid(i,j-1) + curr_grid(i,j+1))
                               

            enddo
        enddo
        
        ! Ajout de la source
        ! i = int(Nx * 0.5)
        ! j = int(Ny * 0.5)
        ! next_grid(i, j) = next_grid(i,j) + dt**2 * amplitude * sin(omega * it * dt) * exp( -((it*dt-5.0)**4 / (2.)**4))

        ! Mise à jour des niveaux de grille
        prev_grid = curr_grid
        curr_grid = next_grid

    end subroutine


    ! __________________________________________________________________________
    !
    ! FR: mise à jour des conditions limites (Dirichlet)
    ! __________________________________________________________________________
    subroutine update_boundaries(grid,        &
                                Nx, Ny)
        implicit none

        integer, intent(in)                    :: Nx, Ny        ! Nombres de noeuds/points de la sous-grille dans chaque direction
        real(8), dimension(:,:), intent(inout) :: grid          ! grille locale

        ! -x boundary conditions
        grid(1,:) = 0.

        ! +x boundary conditions
        grid(Nx,:) = 0.

        ! -y boundary conditions
        grid(:,1) = 0.

        ! +y boundary conditions
        grid(:,Ny) = 0.

    end subroutine

    ! __________________________________________________________________________
    !
    ! Finalisation de la physique et du domaine
    ! __________________________________________________________________________
    subroutine finalise_domain(prev_grid, curr_grid, next_grid)
        implicit none

        ! Liste des grilles
        real(8), dimension(:,:), allocatable, intent(inout) :: prev_grid, curr_grid, next_grid

        ! Desallocation des tableaux dynamiques
        deallocate(next_grid)
        deallocate(curr_grid)
        deallocate(prev_grid)

    end subroutine

end module
