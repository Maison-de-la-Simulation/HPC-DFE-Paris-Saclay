! ______________________________________________________________________________
!
! Exercice 7 : Création d'une topologie cartésienne
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

    ! Déclaration des paramètres

    integer               :: ierror              ! Code d'erreur pour MPI
    integer               :: number_of_ranks     ! Nombre total de rang MPI
    integer               :: rank                ! Le rang du processus en cours
    integer               :: dim = 2             ! Dimension (2 pour 2D)
    integer, dimension(2) :: ranks_per_dimension ! Nombre de rangs dans chaque direction
    integer               :: cartesian_communicator  ! Communicateur cartésien
    logical               :: reorganisation      ! Réorganisation des rangs
    logical, dimension(2) :: periodicity         ! Conditions limites périodiques pour les voisins
    integer, dimension(2) :: rank_coordinates    ! Coordonnées du rang en cours
    integer               :: rank_neighbors_mx   ! rang du voisin en -x
    integer               :: rank_neighbors_my   ! rang du voisin en -y
    integer               :: rank_neighbors_px   ! rang du voisin en +x
    integer               :: rank_neighbors_py   ! rang du voisin en +y
    integer               :: i

    ! Initialisation de certains paramètres

    ranks_per_dimension = (/4,3/)
    reorganisation      = .true.
    periodicity         = (/.true.,.true./)

    ! Initialisation de MPI

    Call MPI_INIT(ierror)

    ! Nombre total de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Création de la topologie cartésienne

    Call MPI_???(???, &  ! Ancien communicateur
                 ???, &  ! Dimension
                 ???, &  ! Nombre de rangs par dimension,
                 ???, &  ! Périodicité des rangs voisins,
                 ???, &  ! Réorganisation des rangs
                 ???, &  ! Nouveau communicateur (cartésien)
                 ierror) ! Code d'erreur

    ! On récupère le rang dans le nouveau communicateur cartésien

    Call MPI_COMM_RANK(cartesian_communicator, rank, ierror)

    ! On récupère les coordonnées du processus courant dans
    ! la topologie cartésienne

    CALL MPI_???(???, &    ! Communicateur cartésien
                        ???, &     ! Rang du processus courant
                        ???, &     ! Dimension
                        ???, &     ! Coordonnées retournées du processus
                        ???)       ! Code d'erreur

    ! On récupère les rangs voisins

    ! En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)
    CALL MPI_???(???, &     ! Communicateur cartésien
                 ???,                      &     ! Direction
                 ???,                      &     ! Déplacement (un pas positif)
                 rank_neighbors_mx,        &     ! Voisin en -x
                 rank_neighbors_px,        &     ! Voisin en +x
                 ierror)

    ! en -y (rank_neighbors_my), en +y (rank_neighbors_py)
    CALL MPI_???(???, &     ! Communicateur cartésien
                 ???,                      &     ! Direction
                 ???,                      &     ! Déplacement (un pas positif)
                 rank_neighbors_my,        &     ! Voisin en -y
                 rank_neighbors_py,        &     ! Voisin en +y
                 ierror)

    ! Chaque processus affiche son rang, ses coordonnées et ses voisins
    ! à tour de rôle

    do i=0,number_of_ranks-1
        if (rank.eq.i) then

            write(0,*)
            write(0,'(X,"Le rang ",I3," a pour coordonnées (",I2,",",I2,")")') &
                 rank,rank_coordinates(1),rank_coordinates(2)
            write(0,'(X,"Il a pour voisins en -x :",I2,", en +x : ",I2,", en -y : ",I2," et en +y : ",I2)') &
                 rank_neighbors_mx,rank_neighbors_px,rank_neighbors_my,rank_neighbors_py

        end if

        Call MPI_BARRIER(cartesian_communicator,ierror)

    end do

    ! Communication de la topologie totale au rang 0
    
    Call MPI_???(???);
    
    ! Affichage de la topologie
    
    if (rank.eq.0) then
    
        write(0,*) ""
        write(0,*) "Carte de la topologie : "
        write(0,*) "-----------------------------------------> y"
    
        do iy = 1,ranks_per_direction(1)
                
                write(line,*) topology_map((iy-1)*ranks_per_direction(2) + 1: (iy)*ranks_per_direction(2))
                
                !write(0,*) rank_char, topology_map((ix-1)*ranks_per_direction(2) + iy)
                write(0,*) "| ",trim(line)
                
        enddo
    
        write(0,*) "v"
        write(0,*) "x"
    
    endif

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
