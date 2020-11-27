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
    
    implicit none
    
    ! ... Ici on pourra mettre l'initialisation MPI ...
    
    ! ... Ici on pourra mettre la creation de la topologie ...
    
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
    
    Call get_arguments( Nt, Nx, Ny, diagnostic_period, dx, Lx, Ly)
    
    ! __________________________________________________________________________
    ! Initialisation de la grille
    
    Call cpu_time(time_1)
    
    Call initialize_domain(Nx, Ny,                    &
                            Lx, Ly,                    &
                            alpha,                     &
                            nt,                        &
                            dt,                        &
                            dx,                        &
                            C,                         &
                            coef,                      &
                            prev_grid, curr_grid, next_grid)
                            
    Call cpu_time(time_2)
    timers(1) = time_2 - time_1

    ! __________________________________________________________________________
    ! Boucle principale

    Call cpu_time(time_1)

    do it=1,Nt
        
        ! On résoud l'équation d'onde sur le pas de temps en cours
        
        Call cpu_time(time_2)
        
        Call update_domain(prev_grid, curr_grid, next_grid, Nx, Ny, coef, dx, dt, Nt, omega, amplitude, it)
        
        Call cpu_time(time_3)
        timers(3) = timers(3) + time_3 - time_2
        
        ! On applique les conditions limites
        
        Call cpu_time(time_2)
        
        Call update_boundaries(curr_grid, Nx, Ny)
        
        Call cpu_time(time_3)
        timers(4) = timers(4) + time_3 - time_2
        
        
        Call cpu_time(time_2)
        
        ! On affiche des informations dans le terminal
        
        Call print_timestep_information(it)
        
        ! On écrit la grille sur le disque pour la visualiser
        
        Call output_grid(curr_grid, it, diagnostic_period, Nx, Ny, Lx, Ly)
        
        Call cpu_time(time_3)
        timers(5) = timers(5) + time_3 - time_2
        
    end do
    
    Call cpu_time(time_2)
    timers(2) = time_2 - time_1
    
    ! __________________________________________________________________________
    ! Bilan du temps passé dans chaque partie
    
    average_timers = timers
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
                    
    Call finalise_domain(prev_grid, curr_grid, next_grid)
                    
end program
