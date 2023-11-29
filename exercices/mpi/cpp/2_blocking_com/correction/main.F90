! ______________________________________________________________________________
!
! Exercice 2 : communication bloquante point à point
!
! Ce second programme présente le fonctionnement des communications
! bloquantes send/recv
! Dans ce code, le processus MPI de rang 1 envoie un message
! au processus de rang 2.
! ______________________________________________________________________________
program blocking_com

    ! il est important d'inclure la bibliothèque
    ! sans quoi aucun de vos appels ne sera reconnu.
    use mpi

    implicit none

    integer               :: provided
    integer               :: ierror             ! Code d'erreur pour MPI
    integer               :: number_of_ranks    ! Nombre total de rang MPI
    integer               :: rank               ! Le rang du processus en cours
    integer               :: message            ! Variable contenant le message
    integer               :: tag

    ! Initialisation de MPI

    Call MPI_INIT_THREAD(MPI_THREAD_SINGLE, &
                        provided,           &
                        ierror)

    ! Nombre total de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Rang du processus MPI

    Call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

    ! Tous les processus ont la variable message initialisée à 0

    message = 0

    ! Envoi du message du rang 1 vers le rang 2

    if (rank.eq.1) then

        message = 42

        write(*,'(X,"Le rang ",I3," envoie le message ",I3," au rang 2 /",I3,".")') &
             rank,message,number_of_ranks

        Call MPI_SEND(message,                  & ! La partie du tableau à envoyer
                      1,                        & ! Le nombre d'élèments
                      MPI_INTEGER,              & ! Le type de donnée utilisé
                      2,                        & ! Le rang du destinataire
                      tag,                      & ! tag de la communication
                      MPI_COMM_WORLD,           & ! Le communicateur
                      ierror)                     ! Code d'erreur

    end if

    ! Reception du message du rang 2 venant du rang 1

    if (rank.eq.2) then

        Call MPI_RECV(message,                  & ! La variable qui récupère le message
                      1,                        & ! Le nombre d'élèments à recevoir
                      MPI_INTEGER,              & ! Le type de donnée envoyé
                      1,                        & ! Le rang de l'expéditeur
                      tag,                      & ! tag de la communication
                      MPI_COMM_WORLD,           & ! Le communicateur
                      MPI_STATUS_IGNORE,        & ! Status de la communication
                      ierror)                     ! Code d'erreur

        write(*,'(X,"Le rang ",I3," a reçu le message ",I3," du rang 1 /",I3,".")') &
             rank,message,number_of_ranks

    end if

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
