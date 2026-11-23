! ______________________________________________________________________________
!
! Exercice 2 : Parallélisation d'une boucle avec openMP
!
! Dans cet exercice, vous apprendrez :
! - à distribuer la charge d'une boucle DO dans une région parallèle
!
! ______________________________________________________________________________
program omp_do
    
    implicit none
    
    real(8), dimension(:), allocatable :: A, B, C, D
    integer                            :: N
    integer                            :: i
    integer                            :: count_1, count_2
    real(8)                            :: cst
    
    N = 100000000
    cst = 15.8
    
    allocate(A(N),B(N),C(N),D(N))
    
    Call system_clock(count_1)
    
    ! Initialisation des tableaux
    Do i = 1,N
        B(i) = cst*cos(2.*i)
        C(i) = cst*sin(3.*i)
        D(i) = cst*cos(4.*i)**2
    End do
    
    ! Boucle à paralléliser
    Do i = 1,N
        A(i) = B(i)**2 + C(i)**3 + C(i)*D(i)*B(i)
    End do
    
    Call system_clock(count_2)
    write(*,'("Temps : ",F10.2, " ms")') real(count_2 - count_1)
    
    deallocate(A,B,C,D)
    
end program
