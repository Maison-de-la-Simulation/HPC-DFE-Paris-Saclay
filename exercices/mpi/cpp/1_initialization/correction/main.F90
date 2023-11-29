! ______________________________________________________________________________
!
! Exercice 1 : Les tout premiers pas avec MPI
!
! Dans cet exercice, vous apprendrez :
! - à initaliser un programme MPI
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
    integer               :: i
    integer               :: N, local_N
    real(8), dimension(:), allocatable :: A, B, C
    real(8)               :: time

    ! Cette première fonction permet d'initialiser MPI,
    ! elle est à appeler avant tout autre action avec MPI.

    Call MPI_INIT(ierror)

    ! Cette subroutine permet de récupérer le nombre total de rangs dans
    ! le communicateur par défaut MPI_COMM_WORLD.

    Call MPI_COMM_SIZE(MPI_COMM_WORLD,  &  ! Le communicateur
                       number_of_ranks, &  ! Le nombre total de rangs
                       ierror)             ! Le code d'erreur

    ! Cette subroutine permet de récupérer le numéro du rang du processus
    ! qui appelle cette subroutine dans
    ! le communicateur par défaut MPI_COMM_WORLD.

    Call MPI_COMM_RANK(MPI_COMM_WORLD,   & ! Le communicateur
                       rank,             & ! Le rang du processus qui appelle cette fonction
                       ierror)             ! Le code erreur

    
    ! Allocation
    
    N = 4000000
    local_N = N / number_of_ranks
    
    allocate(A(local_N))
    allocate(B(local_N))
    allocate(C(local_N))
    
    ! Initialization
    
    Do i = 1,local_N
        
        A(i) = sin(4.*i)
        B(i) = sin(10.*i)**2
        C(i) = cos(5.*i)
        
    Enddo
    
    ! Boucle de calcul
    
    time = MPI_WTIME()
    
    Do i = 2,local_N-1
        
        A(i) = B(i) - C(i) + (B(i-1) + B(i+1) + 4*B(i))
        
    end do
    
    time = MPI_WTIME() - time

    ! Chaque rang affiche son numéro/identifiant (variable rank)

    write(0,'(X,"Je suis le rang ",I3," /",I3, " avec un temps de ", F12.8, " s")') rank,number_of_ranks-1,time

    ! A faire à la fin du programme après tout appel MPI

    Call MPI_FINALIZE(ierror)

end program
