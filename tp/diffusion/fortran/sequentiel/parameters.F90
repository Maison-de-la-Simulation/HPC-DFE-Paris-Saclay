! ______________________________________________________________________________
!
! Module parameters
!
! Ce module contient l'ensemble des paramètres qui seront utilisés dans ce code
! ______________________________________________________________________________
module parameters

    implicit none

    ! __________________________________________________________________________
    ! Paramètres pour MPI
    integer                 :: number_of_ranks               ! Nombre total de rangs MPI
    integer, dimension(2)   :: ranks_per_direction           ! Nombre de rangs MPI dans chaque dimension
    integer                 :: cartesian_communicator
    integer                 :: rank                          ! Numéro d'identification du rang MPI
    integer, dimension(2)   :: rank_coordinates              ! Coordonnées du rang dans la topologie cartésienne
    integer                 :: rank_neighbors_mx             ! Rangs MPI voisins en -x
    integer                 :: rank_neighbors_px             ! Rangs MPI voisins en +x
    integer                 :: rank_neighbors_my             ! Rangs MPI voisins en -y
    integer                 :: rank_neighbors_py             ! Rangs MPI voisins en +y

    ! __________________________________________________________________________
    ! Paramètres physiques
    real(8)                 :: Lx, Ly                        ! Taille du domaine dans chaque dimension
    real(8)                 :: d                             ! Coefficient de diffusion

    ! __________________________________________________________________________
    ! Paramètres de grille globaux
    integer                 :: Nx, Ny                        ! Nombre de points dans chaque direction pour tout le domaine
    real(8)                 :: dx, dy                        ! Pas d'espace

    ! __________________________________________________________________________
    ! Paramètres de temps
    integer                 :: Nt                            ! Nombre d'itération
    real(8)                 :: dt                            ! Pas de temps
    integer                 :: it                            ! Itération temporelle
    real(8)                 :: alpha                         ! Facteur sur le pas de temps pour la condition de stabilité (< 1)

    ! __________________________________________________________________________
    ! Paramètres de grille locaux
    integer                 :: local_Nx, local_Ny            ! Nombre de points dans chaque direction au sein du rang MPI
    real(8)                 :: local_Lx, local_Ly            ! Taille des sous-domaines MPI
    real(8)                 :: local_x_min, local_y_min      ! Minimum locaux pour le rang MPI correspondant
    real(8)                 :: local_x_max, local_y_max      ! Maximum locaux pour le rang MPI correspondant
    real(8), dimension(:,:),  allocatable :: U               ! Grille locale
    real(8), dimension(:,:),  allocatable :: Up              ! Grille locale au temps précédent t

    ! __________________________________________________________________________
    ! Paramètres du schéma numérique
    real(8)                 :: cx, cy                        ! Coefficient numérique

    ! __________________________________________________________________________
    ! Paramètres de diagnostiques
    integer                 :: print_period                  ! Période d'affichage des informations d'itération
    integer                 :: diagnostic_period             ! Période de sortie des diagnostiques
    logical                 :: first_print                   ! Drapeau pour le premier affichage des temps

end module
