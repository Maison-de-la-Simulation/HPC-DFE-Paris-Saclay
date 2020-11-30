! ______________________________________________________________________________
!
! Implémentation de l'équation d'onde en 2D
!
! Méthode : différence finie
! Conditions limites :
!
! Pour en savoir plus :
! - https://www.youtube.com/watch?v=O6fqBxuM-g8
! - https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF2340/v05/foiler/sim04.pdf
! ______________________________________________________________________________

program onde
    
    use parameters
    use diagnostics
    use physics
    use omp_lib
    
    implicit none
    
    
    ! __________________________________________________________________________
    ! Définition des paramètres de simulation par défaut
    
    Nx                  = 500                 ! Nombre de points sur la grille dans la direction x
    Ny                  = 500                 ! Nombre de points sur la grille dans la direction y
    dx                  = 0.01                 ! Pas d'espace
    C                   = 1.                  ! Vitesse du son
    Amplitude           = 40                 ! Amplitude du terme source
    omega               = 2*PI                  ! Fréquence de la perturbation
    Nt                  = 4000                ! Nombre d'itérations temporelles
    alpha               = 0.5                 ! Facteur sur le pas en temps calculé avec la CFL
    print_period        = 500                ! Période de sortie à l'écran des itérations
    diagnostic_period   = 100                 ! Période en nombre d'itération entre chaque sortie de fichier
    first_print         = .true.
   
    Call get_arguments(Nt, Nx,  &
                             Ny, diagnostic_period, dx)
    
    timers = 0
    
    !$omp parallel default(shared) private(count_2,count_3,time_1,time_2,time_3,it,average_timers)
    
    ! __________________________________________________________________________
    ! Initialisation de la grille
    
    time_1 = omp_get_wtime()
    
    !$omp single
    
    Call initialize_domain(Nx, Ny,                    &
                            Lx, Ly,                    &
                            alpha,                     &
                            nt,                        &
                            dt,                        &
                            dx,                        &
                            C,                         &
                            prev_grid, curr_grid, next_grid)
                            
    !$omp end single
                            
    time_2 = omp_get_wtime()
    average_timers(1) = (time_2 - time_1)

    ! __________________________________________________________________________
    ! Boucle principale

    time_1 = omp_get_wtime()

    do it=1,Nt
        
        ! On résoud l'équation d'onde sur le pas de temps en cours
        
        time_2 = omp_get_wtime()
        
        Call update_domain(prev_grid, curr_grid, next_grid, Nx, Ny, C, dx, dt, Nt, omega, amplitude, it)
        
        time_3 = omp_get_wtime()
        
        average_timers(3) = average_timers(3) + (time_3 - time_2)
        
        ! On applique les conditions limites
        
        time_2 = omp_get_wtime()
        
        !$omp single
        
        Call update_boundaries(curr_grid, Nx, Ny)
        
        !$omp end single
        
        time_3 = omp_get_wtime()
        average_timers(4) = average_timers(4) + (time_3 - time_2)
        

        time_2 = omp_get_wtime()
        
        !$omp single
        
        ! On affiche des informations dans le terminal
        
        Call print_timestep_information(it)
        
        ! On écrit la grille sur le disque pour la visualiser
        
        Call output_grid(curr_grid, it, diagnostic_period, Nx, Ny, Lx, Ly)
        
        !$omp end single
        
        time_3 = omp_get_wtime()
        average_timers(5) = average_timers(5) + (time_3 - time_2)
        
        
    end do
    
    time_2 = omp_get_wtime()
    average_timers(2) = (time_2 - time_1)
    
    ! __________________________________________________________________________
    ! Bilan du temps passé dans chaque partie
    
    !$omp master
    
    percentages(:) = average_timers(:) / (average_timers(1) + average_timers(2)) * 100.
    
    write(*,'("")')
    write(*,'(X,"_________________________________________________________________________")')
    write(*,'("")')
    write(*,'("        Timers                                                         ")')
    write(*,'(X,"_________________________________________________________________________")')
    write(*,'(X,"                    |            |            |            |            |")')

    write(*,'(X,A20,4("|",X,A10,X),"|")') "Code part     ","min (s)","mean (s)","max (s)","percentage"
    write(*,'(X,"____________________|____________|____________|____________|____________|")')
    write(*,'(X,"                    |            |            |            |            |")')
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Initialisation",minimum_timers(1),average_timers(1),&
            maximum_timers(1),percentages(1)
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Boucle principale",minimum_timers(2),average_timers(2),&
            maximum_timers(2),percentages(2)
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Total",minimum_timers(2)+minimum_timers(1),average_timers(1)+average_timers(2),&
            maximum_timers(1) + maximum_timers(2),percentages(1)+percentages(2)
            
    print('(X,"____________________|____________|____________|____________|____________|")')
    print*,''
    print*,'        Details of the main loop                                              '
    print('(X,"_________________________________________________________________________")')
    print('(X,"                    |            |            |            |            |")')
            
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Equation d onde",minimum_timers(3),average_timers(3),&
            maximum_timers(3),percentages(3)
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Conditions aux bords",minimum_timers(4),average_timers(4),&
            maximum_timers(4),percentages(4)
    write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
            "Diagnostiques",minimum_timers(5),average_timers(5),&
            maximum_timers(5),percentages(5)

    !$omp end master

    !$omp end parallel
        
end program
