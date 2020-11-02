! ______________________________________________________________________________
!
! Exercice 3 : communication en anneau
!
! Ce second programme présente le fonctionnement des communications
! bloquantes sendrecv.
! On illustre ici son fonctionnement par une communication en anneau.
! Chaque processus envoie un message à son voisin de rang supérieur.
! et reçoit de son voisin de rang inférieur.
! On effectue un anneau périodique.
! ______________________________________________________________________________
program sendrecv

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

    Call MPI_SENDRECV(rank,                       & ! La partie du tableau à envoyer
                      1,                          & ! Le nombre d'élèments
                      MPI_INTEGER,                & ! Le type de donnée utilisé
                      send_rank,                  & ! Le rang du voisin destinataire
                      tag,                        & ! tag de la communication
                      recv_message,               & ! La partie du tableau qui va recevoir les données
                      1,                          & ! Le nombre d'élèments
                      MPI_INTEGER,                & ! Le type de donnée pour les données reçues
                      recv_rank,                  & ! Le rang du voisin qui va nous envoyer des données
                      tag,                        & ! tag de la communication
                      MPI_COMM_WORLD,             & ! Le communicateur
                      MPI_STATUS_IGNORE,          & ! Status de la communication
                      ierror)                       ! Code d'erreur

    ! On affiche les résultats

    write(0,'(X,"Le rang ",I3," a reçu le message ",I3," du rang ",I3,".")') &
         rank,recv_message,recv_rank

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
