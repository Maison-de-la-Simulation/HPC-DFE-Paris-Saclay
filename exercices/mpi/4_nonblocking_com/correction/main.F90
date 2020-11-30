! ______________________________________________________________________________
!
! Exercice 4 : Communication non bloquante point à point
! ______________________________________________________________________________
program nonblocking

    ! il est important d'inclure la bibliothèque
    ! sans quoi aucun de vos appels ne sera reconnu.
    use mpi

    implicit none

    integer               :: ierror             ! Code d'erreur pour MPI
    integer               :: number_of_ranks    ! Nombre total de rang MPI
    integer               :: rank               ! Le rang du processus en cours
    integer               :: recv_message       ! Variable contenant le message reçu
    integer               :: tag
    integer               :: send_rank          ! Le rang a qui on envoie des données
    integer               :: recv_rank          ! Le rang de qui on reçoit
    integer, dimension(2) :: request            ! Numéro de la requête pour le MPI_WAIT

    ! Initialisation de MPI

    Call MPI_INIT(ierror)

    ! Nombre total de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Rang du processus MPI

    Call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

    ! Chaque processus envoie son rang au voisin de rang supérieur et
    ! reçoit donc le rang du voisin de rang inférieur

    ! Calcul des rangs
    ! Rang au bord inférieur
    if (rank.eq.0) then
        send_rank = rank + 1
        recv_rank = number_of_ranks-1
    ! Rang au bord supérieur
    else if (rank.eq.number_of_ranks-1) then
        send_rank = 0
        recv_rank = rank-1
    ! Rang intérieur
    else
        send_rank = rank + 1
        recv_rank = rank - 1
    end if

    write(0,'(X,"Le rang ",I3," envoie le message ",I3," au rang ",I3,".")') &
         rank,rank,send_rank

    CALL MPI_BARRIER(MPI_COMM_WORLD,ierror)

    ! On effectue maintenant les communications

    tag = 0;

    Call MPI_IRECV(recv_message,1,MPI_INTEGER,recv_rank,tag,MPI_COMM_WORLD,request(2),ierror)
    
    Call MPI_ISEND(rank,1,MPI_INTEGER,send_rank,tag,MPI_COMM_WORLD,request(1),ierror)
    
    Call MPI_WAITALL(2,request,MPI_STATUS_IGNORE,ierror)

    ! On affiche les résultats

    write(0,'(X,"Le rang ",I3," a reçu le message ",I3," du rang ",I3,".")') &
         rank,recv_message,recv_rank

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
