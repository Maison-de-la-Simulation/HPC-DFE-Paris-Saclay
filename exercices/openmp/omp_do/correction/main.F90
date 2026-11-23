! ______________________________________________________________________________
!
! Exercice 2 : Parallélisation d'une boucle avec openMP
!
! Dans cet exercice, vous apprendrez :
! - à distribuer la charge d'une boucle DO dans une région parallèle
!
! ______________________________________________________________________________
program omp_do
    
    use omp_lib
    
    implicit none
    
    real(8), dimension(:), allocatable :: A, B, C, D
    integer                            :: N
    integer                            :: i
    real(8)                            :: time_1, time_2
    real(8)                            :: cst
    
    cst = 15.8
    
    N = 100000000
    
    allocate(A(N),B(N),C(N),D(N))
    
    !$omp parallel default(shared) private(i,time_1,time_2)
    
    !$omp master
    time_1 = omp_get_wtime()
    !$omp end master
    
    ! Initialisation des tableaux
    !$omp do schedule(runtime)
    Do i = 1,N
        B(i) = cst*cos(2.*i)
        C(i) = cst*sin(3.*i)
        D(i) = cst*cos(4.*i)**2
    End do
    !$omp end do
    
    ! Boucle à paralléliser
    !$omp do schedule(runtime)
    Do i = 1,N
        A(i) = B(i)**2 + C(i)**3 + D(i)**4 + C(i)*D(i)*B(i)
    End do
    !$omp end do
    
    !$omp master
    time_2 = omp_get_wtime()
    write(*,'("Temps : ",F10.2, " s")') time_2 - time_1
    !$omp end master
    
    !$omp end parallel
    
end program
