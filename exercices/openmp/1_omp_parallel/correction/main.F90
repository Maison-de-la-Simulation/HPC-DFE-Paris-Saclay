! ______________________________________________________________________________
!
! Exercice 1 : Les tout premiers pas OpenMP
!
! Dans cet exercice, vous apprendrez :
! - à créer une région parallèle avec OpenMP
! - à utiliser quelques fonctions de la bibliothèque
!
! ______________________________________________________________________________
program omp_parallel
    
    ! On fait appel à la bibliothèque OpenMP spécifique
    use omp_lib
    
    implicit none
    
    ! Initialisation des variables
    integer :: nombre_de_threads
    integer :: nombre_de_threads_physiques
    integer :: nombre_max_de_threads
    integer :: thread_id
    
    ! Utilisation de la fonction spécifique pour récupérer le nombre maximal de thread possible
    nombre_max_de_threads     = omp_get_max_threads()
    nombre_de_threads_physiques = omp_get_num_procs()
    
    write(*,'("Nombre maximal de thread openmp : ",I5)') nombre_max_de_threads
    write(*,'("Nombre de threads physiques : ",I5)') nombre_de_threads_physiques
    
    !$omp parallel private(thread_id) default(shared)
    
    ! Utilisation de la fonction spécifique pour récupérer le nombre actif de threads
    nombre_de_threads = omp_get_num_threads()
    ! Identifiant du thread courant
    thread_id         = omp_get_thread_num()
    write(*,'("Thread : "I5," / ",I5)') thread_id,nombre_de_threads-1
    
    !$omp end parallel
    
end program
