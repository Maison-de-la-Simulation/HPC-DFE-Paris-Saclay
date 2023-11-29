! ______________________________________________________________________________
!
! Exercice 1 : Les tout premiers pas avec MPI
!
! Dans cet exercice, vous apprendrez :
! - à initialiser un programme MPI
! - à récupérer le nombre total de rangs
! - à récupérer le rang pour chaque processus
!
! ______________________________________________________________________________
program initilization

    ! il est important d'inclure la bibliothèque
    ! sans quoi aucun de vos appels ne sera reconnu.
    use mpi

    implicit none

    integer               :: ierror             ! Code d'erreur pour MPI
    integer               :: number_of_ranks    ! Nombre total de rang MPI
    integer               :: rank               ! Le rang du processus en cours

    ! Cette première fonction permet d'initialiser MPI,
    ! elle est à appeler avant tout autre action avec MPI.

    Call MPI_???(ierror)

    ! Cette subroutine permet de récupérer le nombre total de rangs dans
    ! le communicateur par défaut MPI_COMM_WORLD.

    Call MPI_???(???,            &  ! Le communicateur
                ???,             &  ! Le nombre total de rangs
                ierror)             ! Le code d'erreur

    ! Cette subroutine permet de récupérer le numéro du rang du processus
    ! qui appelle cette subroutine dans
    ! le communicateur par défaut MPI_COMM_WORLD.

    Call MPI_???(???,             & ! Le communicateur
                 ???,             & ! Le rang du processus qui appelle cette fonction
                 ierror)             ! Le code erreur

    ! Chaque rang affiche son numéro/identifiant (variable rank)

    write(0,'(X,"Je suis le rang ",I3," /",I3)') rank,number_of_ranks-1

    ! A faire à la fin du programme après tout appel MPI

    Call MPI_???(ierror)

end program
