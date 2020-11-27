module physics
    
    use parameters
    use mpi
    
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
                                 local_Nx, local_Ny,        &
                                 prev_grid, curr_grid, next_grid)

        implicit none

        integer, intent(in)                  :: Nx, Ny                 ! Nombres de noeuds/points de la grille dans chaque direction
        real(8), intent(inout)               :: Lx, Ly                 ! Taille du domaine dans chaque dimension
        real(8), intent(in)                  :: alpha                  ! Facteur sur le pas de temps pour la condition de stabilité (< 1)
        integer, intent(in)                  :: Nt                     ! Nombre d'itération
        real(8), intent(in)                  :: C                      ! Vitesse du son
        real(8), dimension(:,:), allocatable, intent(out) :: prev_grid ! Tableau représentant la grille structurée au temps précédent
        real(8), dimension(:,:), allocatable, intent(out) :: curr_grid ! Le tableau de la grille structurée au temps courant
        real(8), dimension(:,:), allocatable, intent(out) :: next_grid ! Le tableau de la grille structurée au temps suivant
        real(8), intent(out)                 :: dt                     ! Pas de temps
        real(8), intent(out)                 :: dx                     ! Pas d'espace
        integer, intent(out)                 :: local_Nx, local_Ny
        integer                 :: Nx_min, Ny_min

        real(8) :: r
        integer :: i,j
        integer :: ierror

        if (rank == 0) then
            ! Création du dossier de diagnostique
            Call system("rm -r diags")
            Call system("mkdir -p diags")
        end if
        
        ! Calcul des dimensions à partir des pas et du nombre de points
        Lx = Nx * dx
        Ly = Ny * dx

        ! Calcul du nombre de noeuds dans chaque dimension pour chaque rang MPI
        ! On rajoute deux noeuds pour gérer les cellules fantômes et stocker les noeuds voisins
        local_Nx = Nx / ranks_per_direction(1) + 2
        local_Ny = Ny / ranks_per_direction(2) + 2

        ! Taille des sous-domaine MPI
        local_Lx = (Nx / ranks_per_direction(1))*dx
        local_Ly = (Ny / ranks_per_direction(2))*dx

        ! Calcul des minimums locaux
        local_x_min = rank_coordinates(1)*local_Lx
        local_y_min = rank_coordinates(2)*local_Ly

        ! Calcul des maximums locaux
        local_x_max = local_x_min + local_Lx
        local_y_max = local_y_min + local_Ly


        Nx_min = rank_coordinates(1)*(local_Nx-2) + 1
        Ny_min = rank_coordinates(2)*(local_Ny-2) + 1

        ! Calcul du pas de temps avec respect de la condition de stabilité
        dt = alpha * dx / C

        ! Allocation des grilles, il faut se rappeler que la première
        ! dimension est celle contiguë en mémoire
        allocate(prev_grid(local_Nx,local_Ny))
        allocate(curr_grid(local_Nx,local_Ny))
        allocate(next_grid(local_Nx,local_Ny))

        ! Initialisation des grilles
        prev_grid = 0
        curr_grid = 0
        next_grid = 0

        ! Condition intiale
        Do j=2,local_Ny-1
            Do i=2,local_Nx-1
                r = sqrt((local_x_min + (i-1)*dx - 0.5*Lx)**2 + (local_y_min + (j-1)*dx - 0.5*Ly)**2)
                !r = sqrt(((i+Nx_min-2)*dx - 0.5*Lx)**2 + ((j+Ny_min-2)*dx - 0.5*Ly)**2)
                curr_grid(i,j) = exp(-(r - 0.25*Lx)**2 / (0.1*Lx)**2 )
            End do
        Enddo

        if (rank==0)  then

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
            print('(X,"local Nx: ",(I8,X),"local Ny: ",(I8,X))'),local_Nx, local_Ny
            print('(X,"dt: ",(F8.3,X))'),dt
            print('(X,"Nt: ",(I8,X))'),Nt
            print('(X)')
        
        end if
        
        do i=0,number_of_ranks-1
            if (rank.eq.i) then
                print('(X,"Rang ",I3)'),rank
                print('(X,"Coordinates: ",I3,X,I3)'), rank_coordinates(1), rank_coordinates(2)
                print('(X,"x min: ",F8.3," x max: ",F8.3)'), local_x_min, local_x_max
                print('(X,"y min: ",F8.3," y max: ",F8.3)'), local_y_min, local_y_max
            endif
            Call MPI_BARRIER(cartesian_communicator,ierror)
        enddo
        

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR:
    ! Echange des cellules fantômes
    ! ENG:
    ! Gost cell exchange
    ! __________________________________________________________________________
    subroutine exchange_gost_cells(curr_grid,           &
                                   local_Nx,    &
                                   local_Ny,    &
                                   rank_neighbors_mx, &
                                   rank_neighbors_px, &
                                   rank_neighbors_my, &
                                   rank_neighbors_py, &
                                   cartesian_communicator)

        implicit none

        real(8), dimension(:,:), intent(inout) :: curr_grid
        integer, intent(in)                    :: local_Nx
        integer, intent(in)                    :: local_Ny
        integer, intent(in)                    :: rank_neighbors_mx
        integer, intent(in)                    :: rank_neighbors_px
        integer, intent(in)                    :: rank_neighbors_my
        integer, intent(in)                    :: rank_neighbors_py
        integer, intent(in)                    :: cartesian_communicator

        integer                            :: tag
        integer                            :: ierror

        tag = 0

        ! ______________________________________________________________________
        ! FR: Echange des valeurs en x (axe contigu)
        ! ENG: Exchange x values first (fast axis)

        ! Envoi des données en +x et réception de -x
        Call MPI_SENDRECV(curr_grid(local_Nx-1,2:local_Ny-1), & ! La partie du tableau à envoyer
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_px,          & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          curr_grid(1,2:local_Ny-1),          & ! La partie du tableau qui va recevoir les données
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_mx,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -x et réception de +x
        Call MPI_SENDRECV(curr_grid(2,2:local_Ny-1), & ! La partie du tableau à envoyer
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_mx,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          curr_grid(local_Nx,2:local_Ny-1), & ! La partie du tableau qui va recevoir les données
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_px,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en +y et réception en -y
        Call MPI_SENDRECV(curr_grid(2:local_Nx-1,local_Ny-1), & ! La partie du tableau à envoyer
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_py,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          curr_grid(2:local_Nx-1,1),          & ! La partie du tableau qui va recevoir les données
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_my,        & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -y et réception de +y
        Call MPI_SENDRECV(curr_grid(2:local_Nx-1,2),  & ! La partie du tableau à envoyer
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_my,          & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          curr_grid(2:local_Nx-1,local_Ny),   & ! La partie du tableau qui va recevoir les données
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_py,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

    end subroutine

    ! __________________________________________________________________________
    !
    ! Calcul de l'équation d'onde au pas de temps suivant
    ! __________________________________________________________________________
    subroutine update_domain(prev_grid, curr_grid, next_grid, Nx, Ny, C, dx, dt, Nt, omega, amplitude,  it)
        implicit none

        ! Vitesse du son
        real(8), intent(in)                                 :: C
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
        
        ! Coefficient
        real(8)                                             :: coef
        ! Source
        real(8)                                             :: source

        integer :: i,j

        coef = (c * dt / dx)**2

        ! Schéma différence finie explicite
        do j = 2,local_Ny-1
            do i = 2,local_Nx-1

                next_grid(i,j) = 2*curr_grid(i,j) - prev_grid(i,j) &
                               + coef * (curr_grid(i+1,j) + curr_grid(i-1,j) - 4*curr_grid(i,j) &
                               + curr_grid(i,j-1) + curr_grid(i,j+1))
                               

            enddo
        enddo
        
        ! if (rank == 0) then
        !     ! Ajout de la source
        !     i = local_Nx-1
        !     j = local_Ny-1
        !     next_grid(i, j) = next_grid(i,j) + dt**2 * amplitude * sin(omega * it * dt) * exp( -((it*dt-5.0)**4 / (2.)**4))
        ! end if

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
        if (local_x_min <= 0) then
            grid(2,:) = 0.
        endif

        ! +x boundary conditions
        if (local_x_max >= Lx) then
            grid(Nx-1,:) = 0.
        endif

        ! -y boundary conditions
        if (local_y_min <= 0) then
            grid(:,2) = 0.
        endif

        ! +y boundary conditions
        if (local_y_max >= Ly) then
            grid(:,Ny-1) = 0.
        endif

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
