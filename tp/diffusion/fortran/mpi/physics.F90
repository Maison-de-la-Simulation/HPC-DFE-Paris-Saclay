module physics

    use mpi
    implicit none

contains

    ! __________________________________________________________________________
    !
    ! Initialisation du domaine
    ! __________________________________________________________________________
    subroutine initialize_domain(Nx, Ny,                    &
                                 Lx, Ly,                    &
                                 d, alpha,                  &
                                 nt,                        &
                                 rank,                      &
                                 number_of_ranks,           &
                                 rank_coordinates,          &
                                 ranks_per_direction,       &
                                 communicator,              &
                                 dt,                        &
                                 first_print,               &
                                 dx, dy,                    &
                                 cx, cy,                    &
                                 local_Lx, local_Ly,        &
                                 local_x_min, local_x_max,  &
                                 local_y_min, local_y_max,  &
                                 local_Nx, local_Ny, U, Up)

        implicit none

        integer, intent(in)                  :: Nx, Ny                 ! Nombres de noeuds/points de la grille dans chaque direction
        real(8), intent(in)                  :: Lx, Ly                 ! Taille du domaine dans chaque dimension
        real(8), intent(in)                  :: alpha                  ! Facteur sur le pas de temps pour la condition de stabilité (< 1)
        real(8), intent(in)                  :: d                      ! Coefficient de diffusion
        integer, intent(in)                  :: Nt                     ! Nombre d'itération
        integer, intent(in)                  :: rank                   ! Numéro d'identification du rang MPI
        integer, intent(in)                  :: number_of_ranks        ! Nombre total de rangs MPI
        integer, intent(in), dimension(2)    :: rank_coordinates       ! Coordonnées du rang dans la topologie cartésienne
        integer, dimension(2), intent(in)    :: ranks_per_direction    ! Répartition des rangs dans chaque direction
        integer, intent(in)                  :: communicator           ! Communicateur MPI
        real(8), dimension(:,:), allocatable, intent(out) :: U         ! Tableau représentant la grille structurée
        real(8), dimension(:,:), allocatable, intent(out) :: Up        ! Le tableau de la grille structurée au temps précédent
        integer, intent(out)                 :: local_Nx, local_Ny     ! Nombres locales de noeuds/points de la sous-grille dans chaque direction
        real(8), intent(out)                 :: local_Lx, local_Ly     ! Taille des sous-domaines MPI
        real(8), intent(out)                 :: local_x_min, local_y_min      ! Minimum locaux pour le rang MPI correspondant
        real(8), intent(out)                 :: local_x_max, local_y_max      ! Maximum locaux pour le rang MPI correspondant
        real(8), intent(out)                 :: dt                     ! Pas de temps
        real(8), intent(out)                 :: dx, dy                 ! Pas d'espace
        real(8), intent(out)                 :: cx, cy                 ! Coefficients numériques pour la diffusion
        logical, intent(inout)               :: first_print            ! Drapeau pour le premier affichage des temps

        integer :: i,j
        integer :: ierror
        real(8) :: r,x,y
        real(8) :: rx,ry

        ! Vérification des données rentrées
        if (mod(Nx,ranks_per_direction(1)).ne.0) then
            if (rank == 0) then
                write(0,'("")')
                write(0,'(" ERROR: Nx n''est pas divisible par le nombre de rangs en x")')
                write(0,'("")')
            endif
            Call MPI_BARRIER(communicator,ierror)
            Call MPI_Abort(communicator, 0, ierror)
        end if
        if (mod(Ny,ranks_per_direction(2)).ne.0) then
            if (rank == 0) then
                write(0,'("")')
                write(0,'(" ERROR: Ny n''est pas divisible par le nombre de rangs en y")')
                write(0,'("")')
            endif
            Call MPI_BARRIER(communicator,ierror)
            Call MPI_Abort(communicator, 0, ierror)
        end if

        ! Création du dossier de diagnostique
        if (rank .eq. 0) then
            Call system("mkdir -p diags")
        end if

        ! Paramètre pour les diagnostiques
        first_print = .true.

        ! Calcul du nombre de noeuds dans chaque dimension pour chaque rang MPI
        ! On rajoute deux noeuds pour gérer les cellules fantômes et stocker les noeuds voisins
        local_Nx = Nx / ranks_per_direction(1) + 2
        local_Ny = Ny / ranks_per_direction(2) + 2

        ! Calcul des pas en espace
        dx = Lx / (Nx - 1)
        dy = Ly / (Ny - 1)

        ! Taille des sous-domaine MPI
        local_Lx = (local_Nx-1)*dx
        local_Ly = (local_Ny-1)*dy

        ! Calcul des minimums locaux
        local_x_min = rank_coordinates(1)*local_Lx - dx
        local_y_min = rank_coordinates(2)*local_Ly - dy

        ! Calcul des maximums locaux
        local_x_max = (rank_coordinates(1)+1)*local_Lx + dx
        local_y_max = (rank_coordinates(2)+1)*local_Ly + dy

        ! Calcul du pas de temps avec respect de la condition de stabilité
        dt = alpha * 0.5 / (d / dx**2 + d / dy**2)

        ! Coefficient pour le schéma numérique
        cx = dt*d/(dx**2)
        cy = dt*d/(dy**2)

        ! Allocation de la grille locale, il faut se rappeler que la première
        ! dimension est celle contigue en mémoire
        allocate(U(local_Nx,local_Ny))

        ! Allocation de la grille locale au temps précédent
        allocate(Up(local_Nx,local_Ny))

        rx = 0.2 * Lx
        ry = 0.2 * Ly

        ! Initialisation de la grille
        U(:,:) = 0
        do j = 1,local_Ny

            y = local_y_min + (j-1)*dy - 0.5*Ly

            do i = 1,local_Nx

                x = local_x_min + (i-1)*dx - 0.5*Lx
                r = (x/rx)**2 + (y/ry)**2

                if (r <= 1) then

                    U(i,j) = 1.

                endif

            enddo
        enddo

        ! Bilan des paramètres de simulation
        if (rank == 0) then
            print*, ' '
            print('(X,"______________________________________________________________________________")')
            print('(X)')
            print('(X,"        Code diffusion                                                        ")')
            print('(X,"______________________________________________________________________________")')
            print('(X)')
            print('(X,"dx: ",(F8.3,X),"dy: ",(F8.3,X))'),dx, dy
            print('(X,"cx: ",(F8.3,X),"cy: ",(F8.3,X))'),cx, cy
            print('(X,"Lx: ",(F8.3,X),"Ly: ",(F8.3,X))'),Lx, Ly
            print('(X,"local Lx: ",(F8.3,X),"local Ly: ",(F8.3,X))'),local_Lx, local_Ly
            print('(X,"local Nx: ",(I3,X),"local Ny: ",(I3,X))'),local_Nx, local_Ny
            print('(X,"dt: ",(F8.3,X))'),dt
            print('(X,"Nt: ",(I8,X))'),Nt
            print('(X)')
        endif

        do i=0,number_of_ranks-1
            if (rank.eq.i) then
                print('(X,"Rang ",I3)'),rank
                print('(X,"Coordinates: ",I3,X,I3)'), rank_coordinates(1), rank_coordinates(2)
                print('(X,"x min: ",F8.3," x max: ",F8.3)'), local_x_min, local_x_max
                print('(X,"y min: ",F8.3," y max: ",F8.3)'), local_y_min, local_y_max
            endif
            Call MPI_BARRIER(communicator,ierror)
        enddo

    end subroutine

    ! __________________________________________________________________________
    !
    ! Calcul de l'équation de diffusion au pas de temps suivant
    ! __________________________________________________________________________
    subroutine update_domain(U, Up, local_Nx, local_Ny, cx, cy)
        implicit none

        real(8), intent(in)                                 :: cx, cy    ! Coefficients numériques pour la diffusion
        real(8), dimension(:,:), allocatable, intent(inout) :: U         ! Tableau représentant la grille structurée
        real(8), dimension(:,:), allocatable, intent(inout) :: Up        ! Le tableau de la grille structurée au temps précédent
        integer, intent(in)                    :: local_Nx, local_Ny     ! Nombres locales de noeuds/points de la sous-grille dans chaque direction

        integer :: i,j

        !if (rank == 0) write(0,*) "it",it

        ! Stockage de la grille au temps t pouur actualiser U
        Up = U

        ! Schéma différence finie explicite
        do j = 2,local_Ny-1
            do i = 2,local_Nx-1

                U(i,j) = Up(i,j) + cx*(Up(i+1,j) - 2*Up(i,j) + Up(i-1,j)) &
                                 + cy*(Up(i,j+1) - 2*Up(i,j) + Up(i,j-1))

            enddo
        enddo

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR: mise à jour des conditions limites (Dirichlet)
    ! __________________________________________________________________________
    subroutine update_boundaries(U,                       &
                                local_x_min, local_x_max, &
                                local_y_min, local_y_max, &
                                local_Nx, local_Ny,       &
                                Lx, Ly)
        implicit none

        integer, intent(in)                    :: local_Nx, local_Ny        ! Nombres locales de noeuds/points de la sous-grille dans chaque direction
        real(8), dimension(:,:), intent(inout) :: U                         ! grille locale
        real(8), intent(in)                    :: local_x_min, local_y_min  ! Minimum locaux pour le rang MPI correspondant
        real(8), intent(in)                    :: local_x_max, local_y_max  ! Maximum locaux pour le rang MPI correspondant
        real(8), intent(in)                    :: Lx, Ly                    ! Taille du domaine dans chaque dimension

        ! -x boundary conditions
        if (local_x_min <= 0) then
            U(1,:) = 0.
        end if

        ! +x boundary conditions
        if (local_x_max >= Lx) then
            U(local_Nx,:) = 0.
        end if

        ! -y boundary conditions
        if (local_y_min <= 0) then
            U(:,1) = 0.
        end if

        ! +y boundary conditions
        if (local_y_max >= Ly) then
            U(:,local_Ny) = 0.
        end if

    end subroutine

    ! __________________________________________________________________________
    !
    ! Finalisation de la physique et du domaine
    ! __________________________________________________________________________
    subroutine finalise_domain(U, Up)
        implicit none

        real(8), dimension(:,:), allocatable, intent(inout) :: U         ! Tableau représentant la grille structurée
        real(8), dimension(:,:), allocatable, intent(inout) :: Up        ! Le tableau de la grille structurée au temps précédent

        ! Desallocation des tableaux dynamiques
        deallocate(U)
        deallocate(Up)

    end subroutine

end module
