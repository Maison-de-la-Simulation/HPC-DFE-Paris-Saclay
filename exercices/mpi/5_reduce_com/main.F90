! ______________________________________________________________________________
!
! Exercice 5 : Utilisation des communications collectives MPI_REDUCE
!
!
! Dans cet exercice, nous intégrons la fonction f(x) = sin(x) de
! manière numérique entre 0 et pi/2 et de manière parallèle
! On utilise une intégration par la méthode des rectangles.
! Le domaine est subdivisé suivant le nombre de processus MPI demandé.
!
!
!          delta
! 0         <>                                          pi/2
! |                 |                 |                 |
! |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
! |                 |                 |                 |
!  Premier domaine    deuxième domaine  troisième domaine
!  MPI
!  <--------------->
!    rank_length
!  |                |
! min              max
!
! ______________________________________________________________________________
program reduce

    ! il est important d'inclure la bibliothèque
    ! sans quoi aucun de vos appels ne sera reconnu.
    use mpi

    implicit none

    real(8), parameter    :: pi = 3.1415926535897932   ! Le nombre pi
    integer               :: discretization_per_rank   ! Discrétisation par processus
    real(8)               :: rank_length               ! Taille du sous-domaine d'un rang
    real(8)               :: min                       ! Minimum local du rang
    real(8)               :: max                       ! Maximum local du rang
    real(8)               :: delta                     ! Taille d'un rectangle
    real(8)               :: x                         ! Variable d'intégration
    real(8)               :: local_integration         ! Intégration dans chaque rang
    real(8)               :: integration         ! Valeur finale de l'intégration
    integer               :: ierror             ! Code d'erreur pour MPI
    integer               :: number_of_ranks    ! Nombre total de rang MPI
    integer               :: rank               ! Le rang du processus en cours
    integer               :: i

    ! Initialisation de MPI

    Call MPI_INIT(ierror)

    ! Nombre total de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Rang du processus MPI

    Call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

    ! Paramètres globaux

    discretization_per_rank = 1000                      ! Discrétisation (nombre de points) par processus
    rank_length             = 0.5*pi / number_of_ranks  ! Taille d'un rang

    ! Paramètres locaux

    min   = rank * rank_length                          ! Le minimum d'intégration du processus courant
    max   = (rank+1) * rank_length                      ! La borne maximale d'intégration du processus courant
    delta = rank_length / discretization_per_rank       ! La taille des rectangles (taille du sous-domaine divisé par la discrétisation)

    ! On affiche à l'écran quelques paramètres

    write(0,'(X,"Le rang ",I3," s''occupe de la portion comprise entre",F8.3," et ",F8.3)') &
         rank,min,max

    ! On attend tout le monde

    Call MPI_BARRIER(MPI_COMM_WORLD,ierror)

    if (rank.eq.0) write(0,*)

    ! Intégration locale (chaque processus traite sa partie)

    ! On boucle sur l'ensemble des points du sous-domaine MPI
    do i=1,discretization_per_rank
        x = min + (i-0.5)*delta
        local_integration = local_integration + sin(x)*delta
    enddo

    ! On affiche les intégrations partielles

    write(0,'(X,"Le rang ",I3," a pour intégration locale ",F8.3)') &
         rank,local_integration

    ! Réduction de `local_integration ` pour avoir la valeur finale
    ! dans la variable `integration` sur le rang 0

    Call MPI_???(???,                 & ! valeur local à réduire
                    ???,                 & ! valeur finale
                    ???,                 & ! Nombre d'éléments
                    ???,             & ! Type de donnée échangé
                    ???,             & ! Type de réduction
                    ???,                   & ! Rang de destination de la réduction
                    ???,      & ! Communicateur
                    ierror)                ! Code d'erreur

    ! Affichage du résultat depuis le rang 0

    if (rank.eq.0) then

        write(0,*)
        write(0,'(X,"Le rang ",I3," a pour résultat final ",F8.3)') &
             rank,integration

    end if

    ! Finalisation de MPI

    Call MPI_FINALIZE(ierror)

end program
