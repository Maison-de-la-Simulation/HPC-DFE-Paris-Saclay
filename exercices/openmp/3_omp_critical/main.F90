! ______________________________________________________________________________
!
! Exercice 3 : gestion des zones critiques
!
! Dans cet exercice, vous apprendrez :
! - à utiliser une zone critique
!
!
! ______________________________________________________________________________
program omp_do
    
    use omp_lib
    
    implicit none
    
    integer  :: N
    real(8)  :: sum = 0
    real(8)  :: time_0, time_1
    integer  :: id
    
    !#omp parallel private(id, time_0, time_1) default(shared)
    
    id = omp_get_thread_num();
    
    time_0 = omp_get_wtime();
    
    ! Chaque thread ajoute à sum sa contribution N fois
    Do i = 1,N
        sum += cos(2*id*i)
    End do
    
    time_1 = omp_get_wtime();
    
    write(*,'(" Thread : ",F10.2, " s")') real(time_1 - time_0)
    
    !# omp end parallel
    
    write(*,'(" Sum : ",F10.2)') sum
    
end program
