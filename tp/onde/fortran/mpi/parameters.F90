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
    integer                 :: rank                          ! Numéro d'identification du rang MPI
    logical, dimension(2)   :: periodicity                   ! La périodicité aux bords
    logical                 :: reorganisation                ! Réorganisation des rangs dans le nouveau communicateur cartésien
    integer, dimension(2)   :: ranks_per_direction           ! Nombre de rangs MPI dans chaque dimension
    integer                 :: cartesian_communicator
    integer, dimension(2)   :: rank_coordinates              ! Coordonnées du rang dans la topologie cartésienne
    integer                 :: rank_neighbors_mx             ! Rangs MPI voisins en -x
    integer                 :: rank_neighbors_px             ! Rangs MPI voisins en +x
    integer                 :: rank_neighbors_my             ! Rangs MPI voisins en -y
    integer                 :: rank_neighbors_py             ! Rangs MPI voisins en +y
    
    ! __________________________________________________________________________
    ! Paramètres physiques
    real(8)                 :: Lx, Ly                        ! Taille du domaine dans chaque dimension
    real(8)                 :: C                             ! Vitesse du son
    real(8)                 :: omega                         ! Fréquence de la perturbation
    real(8)                 :: amplitude                     ! Amplitude du terme source
    real(8), parameter      :: PI=4.*ATAN(1.)                ! Pi

    ! __________________________________________________________________________
    ! Paramètres de grille globaux
    integer                 :: Nx, Ny                        ! Nombre de points dans chaque direction pour tout le domaine
    real(8)                 :: dx                            ! Pas d'espace

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

    real(8), dimension(:,:),  allocatable :: next_grid      ! Grille locale
    real(8), dimension(:,:),  allocatable :: curr_grid      ! Grille locale
    real(8), dimension(:,:),  allocatable :: prev_grid      ! Grille locale au temps précédent t

    ! __________________________________________________________________________
    ! Paramètres du schéma numérique
    real(8)                 :: cx, cy                        ! Coefficient numérique

    ! __________________________________________________________________________
    ! Paramètres de diagnostiques
    integer                 :: print_period                  ! Période d'affichage des informations d'itération
    integer                 :: diagnostic_period             ! Période de sortie des diagnostiques
    logical                 :: first_print = .true.          ! Drapeau pour le premier affichage des temps

    ! __________________________________________________________________________
    ! Paramètre pour calculer le temps
    real(8)                 :: time_1, time_2, time_3
    integer                 :: count_1, count_2, count_3
    real(8), dimension(10)  :: timers
    real(8), dimension(10)  :: minimum_timers
    real(8), dimension(10)  :: average_timers
    real(8), dimension(10)  :: maximum_timers
    real(8), dimension(10)  :: percentages

    contains
        
    ! __________________________________________________________________________
    !
    ! Get the command line arguments
    ! __________________________________________________________________________
    subroutine get_arguments( Nt, Nx,  &
                             Ny, diagnostic_period, dx, Lx, Ly, ranks_per_direction) !ranks_per_direction

        implicit none

        integer, dimension(2), intent(inout)    :: ranks_per_direction    ! Répartition des rangs dans chaque direction
        integer, intent(inout)                  :: Nt                     ! Nombre d'itération
        integer, intent(inout)                  :: Nx, Ny                 ! Nombre de points dans chaque direction pour tout le domaine
        integer, intent(inout)                  :: diagnostic_period      ! Période en itération entre chaque sortie
        real(8), intent(inout)                  :: dx                      ! Pas d'espace
        real(8), intent(inout)                  :: Lx, Ly                 ! Taille du domaine dans la direction x puis y (grandeur arbitraire)

        integer           :: number_of_arguments
        integer           :: i
        character(len=32) :: arg1, arg2, arg3

        number_of_arguments = COMMAND_ARGUMENT_COUNT()

        if (number_of_arguments > 0) then

            i = 1
            do while (i<number_of_arguments)
                Call GET_COMMAND_ARGUMENT(i,arg1)

                if (trim(arg1).eq."-ranksxy") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    Call GET_COMMAND_ARGUMENT(i+2,arg3)
                    read(arg2,*) ranks_per_direction(1)
                    read(arg3,*) ranks_per_direction(2)
                    i = i + 3
                end if

                if (trim(arg1).eq."-Nt") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) Nt
                    i = i + 2
                end if

                if (trim(arg1).eq."-Nxy") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    Call GET_COMMAND_ARGUMENT(i+2,arg3)
                    read(arg2,*) Nx
                    read(arg3,*) Ny
                    i = i + 3
                end if

                if (trim(arg1).eq."-diags") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) diagnostic_period
                    i = i + 2
                end if

                if (trim(arg1).eq."-dx") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) dx
                    i = i + 2
                end if

            end do

        end if

    end subroutine

end module
