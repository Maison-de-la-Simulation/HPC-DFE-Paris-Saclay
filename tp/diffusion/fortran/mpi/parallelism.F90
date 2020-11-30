! ______________________________________________________________________________
!
! Implémentation de l'équation de diffusion 2D
! Module Parallelism
!
! Description : ce module traite du parallèlisme dans cette mini-application.
! Il contient toutes les fonctions propres au parallèlisme distribué MPI utilisé.
!
! ______________________________________________________________________________

module parallelism

    use mpi
    use timers

    implicit none

contains

    ! __________________________________________________________________________
    !
    ! Initialisation de MPI
    ! __________________________________________________________________________
    subroutine initialize_mpi(number_of_ranks, rank)

        implicit none
        integer, intent(out)  :: number_of_ranks
        integer, intent(out)  :: rank
        integer               :: provided
        integer               :: ierror

        Call MPI_INIT_THREAD(MPI_THREAD_SINGLE,provided,ierror)

        ! Récupération du nombre de rangs

        Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

        ! Récupération du rang du processus

        Call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR:
    ! Creation de la topologie cartésienne
    ! ENG:
    ! Create the MPI Cartesian decomposition
    ! __________________________________________________________________________
    subroutine create_cartesian_topology(rank,                &
                                         ranks_per_direction, &
                                         rank_coordinates,    &
                                         rank_neighbors_mx,   &
                                         rank_neighbors_px,   &
                                         rank_neighbors_my,   &
                                         rank_neighbors_py,   &
                                         cartesian_communicator)

        implicit none

        integer, intent(inout)               :: rank                     ! Le rang du processus courant
        integer, dimension(2), intent(in) :: ranks_per_direction      ! Le nombre de rangs dans chaque direction x et y
        integer, dimension(2), intent(out):: rank_coordinates         ! Les coordonnées de chaque rang dans la topologie cartésienne
        integer, intent(out)              :: rank_neighbors_my        ! Le rang voisin en -y
        integer, intent(out)              :: rank_neighbors_py        ! Le rang voisin en +y
        integer, intent(out)              :: rank_neighbors_mx        ! Le rang voisin en -x
        integer, intent(out)              :: rank_neighbors_px        ! Le rang voisin en +x
        integer, intent(out)              :: cartesian_communicator   ! Communicateur cartésien généré par cette fonction

        integer                           :: ierror
        logical, dimension(2)             :: periodicity              ! La périodicité aux bords
        logical                           :: reorganisation           ! Réorganisation des rangs dans le nouveau communicateur cartésien

        ! On impose des conditions de Dirichlet, nous ne sommes donc pas périodique
        periodicity    = (/.false.,.false./)

        ! Réorganisation des rangs dans le nouveau communicateur
        reorganisation = .true.

        ! Vérification de la cohérence des paramètres
        if (ranks_per_direction(1)*ranks_per_direction(2).ne.number_of_ranks) then
            if (rank == 0) then
                write(0,'("")')
                write(0,'(" ERROR: La répartition des rangs dans chaque direction")')
                write(0,'("        ne correspond pas au nombre total de processus")')
                write(0,'("        demandé.")')
                write(0,'("")')
            endif
            Call MPI_BARRIER(MPI_COMM_WORLD,ierror)
            Call MPI_Abort(MPI_COMM_WORLD, 0, ierror)
        endif

        ! Creation de la topology cartésienne
        Call MPI_CART_CREATE(MPI_COMM_WORLD,      & ! Ancien communicateur
                             2,                   & ! Dimension
                             ranks_per_direction, & ! Nombre de rangs par dimension,
                             periodicity,         & ! Periodicité des rangs voisins,
                             reorganisation,      & ! Réorganisation des rangs
                             cartesian_communicator,& ! Nouveau communicateur (cartésien)
                             ierror)                  ! Code d'erreur

        ! On récupère le rang dans le nouveau communicateur cartésien
        Call MPI_COMM_RANK(cartesian_communicator, &
                           rank,                   &
                           ierror)

        ! On obtient les coordonnées des rangs MPI dans la topologie cartésienne
        CALL MPI_CART_COORDS(cartesian_communicator, &    ! Communicateur cartésien
                            rank, &                       ! Rang du processus courant
                            2, &                          ! Dimension
                            rank_coordinates, &           ! Coordonnées retournées du processus
                            ierror)                       ! Code d'erreur

        ! On récupère les rangs voisins

        ! en -y (rank_neighbors_my), en +y (rank_neighbors_py)
        CALL MPI_CART_SHIFT(cartesian_communicator, &     ! Communicateur cartésien
                            1,                      &     ! Direction
                            1,                      &     ! Déplacement (un pas positif)
                            rank_neighbors_my,      &     ! Voisin en -y
                            rank_neighbors_py, &          ! Voisin en +y
                            ierror)

        ! En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)
        CALL MPI_CART_SHIFT(cartesian_communicator, &     ! Communicateur cartésien
                            0,                      &     ! Direction
                            1,                      &     ! Déplacement (un pas positif)
                            rank_neighbors_mx,      &     ! Voisin en -x
                            rank_neighbors_px, &          ! Voisin en +x
                            ierror)

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR:
    ! Echange des cellules fantômes
    ! ENG:
    ! Gost cell exchange
    ! __________________________________________________________________________
    subroutine exchange_gost_cells(U,           &
                                   local_Nx,    &
                                   local_Ny,    &
                                   rank_neighbors_mx, &
                                   rank_neighbors_px, &
                                   rank_neighbors_my, &
                                   rank_neighbors_py, &
                                   cartesian_communicator)

        implicit none

        real(8), dimension(:,:), intent(inout) :: U
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

        call timers_mark(3)

        ! ______________________________________________________________________
        ! FR: Echange des valeurs en x (axe contigu)
        ! ENG: Exchange x values first (fast axis)

        ! Envoi des données en +x et réception de -x
        Call MPI_SENDRECV(U(local_Nx-1,2:local_Ny-1), & ! La partie du tableau à envoyer
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_px,          & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(1,2:local_Ny-1),          & ! La partie du tableau qui va recevoir les données
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_mx,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -x et réception de +x
        Call MPI_SENDRECV(U(2,2:local_Ny-1), & ! La partie du tableau à envoyer
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_mx,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(local_Nx,2:local_Ny-1), & ! La partie du tableau qui va recevoir les données
                          local_Ny-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_px,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en +y et réception en -y
        Call MPI_SENDRECV(U(2:local_Nx-1,local_Ny-1), & ! La partie du tableau à envoyer
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_py,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(2:local_Nx-1,1),          & ! La partie du tableau qui va recevoir les données
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_my,        & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -y et réception de +y
        Call MPI_SENDRECV(U(2:local_Nx-1,2),          & ! La partie du tableau à envoyer
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_my,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(2:local_Nx-1,local_Ny),   & ! La partie du tableau qui va recevoir les données
                          local_Nx-2,                 & ! Le nombre d'élèments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_py,        & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Status de la communication
                          ierror)                       ! Code d'erreur

        call timers_add(3)

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR: Calcule l'intégrale de la grille
    ! ENG: Compute the integration of the grid
    ! __________________________________________________________________________
    subroutine compute_grid_integration(integration)
        implicit none

        integer              :: i,j
        real(8)              :: rank_integration
        real(8), intent(out) :: integration
        integer              :: ierror

        integration = 0.
        rank_integration = 0.

        ! Boucle sur les indices y
        do j=2,local_Ny-1
            ! Boucle sur indices en x
            do i=2,local_Nx - 1
                rank_integration = rank_integration + U(i,j)*dx*dy
            enddo
        enddo

        ! Reduction in rank 0
        CALL MPI_REDUCE(rank_integration, integration, 1, &
                        MPI_DOUBLE_PRECISION, MPI_SUM, 0, cartesian_communicator,ierror)

    end subroutine

    ! __________________________________________________________________________
    !
    ! FR:
    ! Finalisation de MPI
    ! ENG:
    ! MPI finalization
    ! __________________________________________________________________________
    subroutine finalize_mpi()

        implicit none
        integer :: ierror

        CALL MPI_FINALIZE(ierror)

    end subroutine

end module
