! ______________________________________________________________________________
!
! Exercice 6 : Création d'une topologie cartésienne
!
! Cet exercice consiste à mettre en place une topologie cartésienne 2D à
! partir des fonctions MPI adéquates.
!
! ______________________________________________________________________________

program cartesian_com

    ! il est important d'inclure la bibliothèque
    ! sans quoi aucun de vos appels ne sera reconnu.
    use mpi

    implicit none

    integer               :: ierror              ! Code d'erreur pour MPI
    integer               :: number_of_ranks     ! Nombre total de rang MPI
    integer               :: rank                ! Le rang du processus en cours
    integer               :: dim = 2             ! Dimension (2 pour 2D)
    integer, dimension(2) :: ranks_per_direction ! Nombre de rangs dans chaque direction
    integer               :: cartesian_communicator  ! Communicateur cartésien
    logical               :: reorganisation      ! Réorganisation des rangs
    logical, dimension(2) :: periodicity         ! Conditions limites périodiques pour les voisins
    integer, dimension(2) :: rank_coordinates    ! Coordonnées du rang en cours
    integer               :: rank_neighbors_mx   ! rang du voisin en -x
    integer               :: rank_neighbors_my   ! rang du voisin en -y
    integer               :: rank_neighbors_px   ! rang du voisin en +x
    integer               :: rank_neighbors_py   ! rang du voisin en +y
    integer               :: i

    ! Paramètres

    ranks_per_direction = (/3,4/)
    reorganisation      = .true.
    periodicity         = (/.true.,.true./)

    ! Initialisation de MPI

    Call MPI_INIT(ierror)

    ! Nombre total de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Creation de la topology cartésienne

    Call MPI_CART_CREATE(MPI_COMM_WORLD,      &
                         2,                   &
                         ranks_per_direction, &
                         periodicity,         &
                         reorganisation,      &
                         cartesian_communicator,&
                         ierror)

    ! On récupère le rang dans le nouveau communicateur cartésien

    Call MPI_COMM_RANK(cartesian_communicator, rank, ierror)

    ! On récupère les coordonnées du processus courant dans
    ! la topologie cartésienne

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

    ! Chaque processus affiche son rang, ses coordonnées et ses voisins
    ! à tour de rôle

    do i=0,number_of_ranks-1
        if (rank.eq.i) then

            write(0,*)
            write(0,'(X,"Le rang ",I3," a pour coordonnées (",I2,",",I2,")")') &
                 rank,rank_coordinates(1),rank_coordinates(2)
            write(0,'(X,"Il a pour voisins en -x :",I2,", en +x",I2,", en -y : ",I2," et en +y : ",I2)') &
                 rank_neighbors_mx,rank_neighbors_px,rank_neighbors_my,rank_neighbors_py

        end if

        Call MPI_BARRIER(cartesian_communicator,ierror)

    end do

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
