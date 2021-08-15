! ______________________________________________________________________________
!
! Implémentation de l'équation de diffusion 2D
! Module Parallelism
!
! Description : ce module traite du parallélisme dans cette mini-application.
! Il contient toutes les fonctions propres au parallélisme distribué MPI utilisé.
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
        integer, intent(out)  :: number_of_ranks     ! Le nombre total de rangs
        integer, intent(out)  :: rank                ! Le rang du processus courant
        integer               :: ierror              ! Le code erreur

        ! Initilisation de MPI

        Call MPI_???(ierror)

        ! Récupération du nombre de rangs

        Call MPI_???(???,            &  ! Le communicateur par défaut
                    ???,             &  ! Le nombre total de rangs
                    ierror)             ! Le code d'erreur

        ! Récupération du rang du processus courant

        Call MPI_???(???,             & ! Le communicateur
                 ???,                 & ! Le rang du processus qui appelle cette fonction
                 ierror)             ! Le code erreur

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

        integer, intent(inout)            :: rank                     ! Le rang du processus courant
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

        ! Creation de la topologie cartésienne
        Call MPI_???(???,      & ! Ancien communicateur
                     ???,                   & ! Dimension
                     ???,         & ! Nombre de rangs par direction,
                     ???,         & ! Périodicité aux bords,
                     ???,         & ! Réorganisation des rangs
                     ???,         & ! Nouveau communicateur (cartésien)
                     ierror)                  ! Code d'erreur

        ! On récupère le rang du processus courant dans le nouveau communicateur cartésien
        Call MPI_???(???,      & ! Le nouveau communicateur cartésien
                     ???,      & ! La variable qui stocke le rang du processus courant
                     ierror)

        ! On obtient les coordonnées des rangs MPI dans la topologie cartésienne
        CALL MPI_???(???,      & ! Le nouveau communicateur cartésien
                    ???, &                       ! Rang du processus courant
                    ???, &                          ! Dimension
                    ???, &           ! Coordonnées retournées du processus
                    ierror)                       ! Code d'erreur

        ! On récupère les rangs voisins

        ! En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)
        CALL MPI_???(???, &     ! Communicateur cartésien
                            ???, &     ! Direction pour x
                            ???, &     ! Déplacement (un pas positif)
                            ???,      &     ! Voisin en -x
                            ???, &          ! Voisin en +x
                            ierror)

        ! en -y (rank_neighbors_my), en +y (rank_neighbors_py)
        CALL MPI_???(???, &     ! Communicateur cartésien
                            ???, &     ! Direction pour y
                            ???, &     ! Déplacement (un pas positif)
                            ???, &     ! Voisin en -y
                            ???, &          ! Voisin en +y
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

        real(8), dimension(:,:), intent(inout) :: U                    ! La sous-grille du processus en cours
        integer, intent(in)                    :: local_Nx             ! Le nombre de points en x de la sous-grille
        integer, intent(in)                    :: local_Ny             ! Le nombre de points en y de la sous-grille
        integer, intent(in)                    :: rank_neighbors_mx    ! Le rang voisin en -x
        integer, intent(in)                    :: rank_neighbors_px    ! Le rang voisin en +x
        integer, intent(in)                    :: rank_neighbors_my    ! Le rang voisin en -y
        integer, intent(in)                    :: rank_neighbors_py    ! Le rang voisin en +y
        integer, intent(in)                    :: cartesian_communicator  ! Le communicateur cartésien

        integer                            :: tag
        integer                            :: ierror

        tag = 0

        call timers_mark(3)

        ! ______________________________________________________________________
        ! FR: Echange des valeurs en x (axe contigu)
        ! ENG: Exchange x values first (fast axis)

        ! Envoi des données en +x et réception de -x
        Call MPI_???(???, & ! La partie du tableau à envoyer
                     ???, & ! Le nombre d'éléments
                     ???, & ! Le type de donnée utilisé
                     ???, & ! Le rang du voisin destinataire
                     tag, & ! tag de la communication
                     ???, & ! La partie du tableau qui va recevoir les données
                     ???, & ! Le nombre d'éléments
                     ???, & ! Le type de donnée pour les données reçues
                     ???, & ! Le rang du voisin qui va nous envoyer des données
                     tag, & ! tag de la communication
                     ???, & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Statut de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -x et réception de +x
        Call MPI_???(U(2,2:local_Ny-1), & ! La partie du tableau à envoyer
                          local_Ny-2,                 & ! Le nombre d'éléments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_mx,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(local_Nx,2:local_Ny-1), & ! La partie du tableau qui va recevoir les données
                          local_Ny-2,                 & ! Le nombre d'éléments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_px,          & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Statut de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en +y et réception en -y
        Call MPI_???(???, & ! La partie du tableau à envoyer
                     ???,                 & ! Le nombre d'éléments
                     ???,       & ! Le type de donnée utilisé
                     ???,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                     ???,          & ! La partie du tableau qui va recevoir les données
                     ???,                 & ! Le nombre d'éléments
                     ???,       & ! Le type de donnée pour les données reçues
                     ???,        & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                     ???,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Statut de la communication
                          ierror)                       ! Code d'erreur

        ! Envoi des données en -y et réception de +y
        Call MPI_???(U(2:local_Nx-1,2),          & ! La partie du tableau à envoyer
                          local_Nx-2,                 & ! Le nombre d'éléments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée utilisé
                          rank_neighbors_my,        & ! Le rang du voisin destinataire
                          tag,                        & ! tag de la communication
                          U(2:local_Nx-1,local_Ny),   & ! La partie du tableau qui va recevoir les données
                          local_Nx-2,                 & ! Le nombre d'éléments
                          MPI_DOUBLE_PRECISION,       & ! Le type de donnée pour les données reçues
                          rank_neighbors_py,        & ! Le rang du voisin qui va nous envoyer des données
                          tag,                        & ! tag de la communication
                          cartesian_communicator,     & ! Le communicateur
                          MPI_STATUS_IGNORE,          & ! Statut de la communication
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

        integration = 0.                    ! ! Variable d'intégration globale
        rank_integration = 0.               ! Variable d'intégration locale

        ! Boucle sur les indices y
        do j=2,local_Ny-1
            ! Boucle sur indices en x
            do i=2,local_Nx - 1
                rank_integration = rank_integration + U(i,j)*dx*dy
            enddo
        enddo

        ! Reduction dans le rang 0
        CALL MPI_???(???, &   ! Variable d'intégration locale
                        ???, &        ! Variable d'intégration globale
                        ???, &                  ! Taille
                        ???, & ! Type de variable
                        ???, & ! Type de réduction
                        0,   & ! Le rang qui reçoit les résultats
                        cartesian_communicator, ierror)

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
