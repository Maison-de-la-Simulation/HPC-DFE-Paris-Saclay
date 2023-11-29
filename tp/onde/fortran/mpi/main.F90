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
    
    use mpi
    use parameters
    use diagnostics
    use physics
    
    implicit none
    
    integer :: ierror
    
    ! __________________________________________________________________________
    ! Initialisation de MPI
    
    Call MPI_INIT(ierror)

    ! Récupération du nombre de rangs

    Call MPI_COMM_SIZE(MPI_COMM_WORLD, number_of_ranks, ierror)

    ! Récupération du rang du processus

    Call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)
    
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
    ranks_per_direction = (/2,2/)
    
    Call get_arguments( Nt, Nx,  &
                             Ny, diagnostic_period, dx, Lx, Ly, ranks_per_direction)
    
    ! __________________________________________________________________________
    ! Topologie Cartésienne
    
    ! On impose des conditions de Dirichlet, nous ne sommes donc pas périodique
    periodicity    = (/.false.,.false./)

    ! Réorganisation des rangs dans le nouveau communicateur
    reorganisation = .true.
    
    ! Vérification de la cohérence des paramètres
    if (ranks_per_direction(1)*ranks_per_direction(2).ne.number_of_ranks) then
        if (rank == 0) then
            write(0,'("")')
            write(0,'(" ERROR: La répartition des rangs dans chaque direction")')
            write(0,'("        ne correspond pas au nombre total de processus")')
            write(0,'("        demandé.")')
            write(0,'("")')
        endif
        Call MPI_BARRIER(MPI_COMM_WORLD,ierror)
        Call MPI_Abort(MPI_COMM_WORLD, 0, ierror)
    endif
    
    ! Creation de la topology cartésienne
    Call MPI_CART_CREATE(MPI_COMM_WORLD,      & ! Ancien communicateur
                         2,                   & ! Dimension
                         ranks_per_direction, & ! Nombre de rangs par dimension,
                         periodicity,         & ! Periodicité des rangs voisins,
                         reorganisation,      & ! Réorganisation des rangs
                         cartesian_communicator,& ! Nouveau communicateur (cartésien)
                         ierror)                  ! Code d'erreur
    
    ! On récupère le rang dans le nouveau communicateur cartésien
    Call MPI_COMM_RANK(cartesian_communicator, &
                    rank,                   &
                    ierror)

    ! On obtient les coordonnées des rangs MPI dans la topologie cartésienne
    CALL MPI_CART_COORDS(cartesian_communicator, &    ! Communicateur cartésien
                     rank, &                       ! Rang du processus courant
                     2, &                          ! Dimension
                     rank_coordinates, &           ! Coordonnées retournées du processus
                     ierror)                       ! Code d'erreur
    
    ! On récupère les rangs voisins

    ! en -y (rank_neighbors_my), en +y (rank_neighbors_py)
    CALL MPI_CART_SHIFT(cartesian_communicator, &     ! Communicateur cartésien
                     1,                      &     ! Direction
                     1,                      &     ! Déplacement (un pas positif)
                     rank_neighbors_my,      &     ! Voisin en -y
                     rank_neighbors_py, &          ! Voisin en +y
                     ierror)

    ! En -x (rank_neighbors_mx) et en +x (rank_neighbors_px)
    CALL MPI_CART_SHIFT(cartesian_communicator, &     ! Communicateur cartésien
                     0,                      &     ! Direction
                     1,                      &     ! Déplacement (un pas positif)
                     rank_neighbors_mx,      &     ! Voisin en -x
                     rank_neighbors_px, &          ! Voisin en +x
                     ierror)
    
    ! __________________________________________________________________________
    ! Initialisation de la grille
    
    time_1 = mpi_wtime()
    
    Call initialize_domain(Nx, Ny,                    &
                            Lx, Ly,                    &
                            alpha,                     &
                            nt,                        &
                            dt,                        &
                            dx,                        &
                            C,                         &
                            local_Nx, local_Ny,        &
                            prev_grid, curr_grid, next_grid)
                            
    time_2 = mpi_wtime()
    
    timers(1) = time_2 - time_1

    ! __________________________________________________________________________
    ! Boucle principale

    time_1 = mpi_wtime()

    do it=1,Nt
        
        ! On résoud l'équation d'onde sur le pas de temps en cours
        
        time_2 = mpi_wtime()
        
        Call exchange_gost_cells(curr_grid,           &
                                       local_Nx,    &
                                       local_Ny,    &
                                       rank_neighbors_mx, &
                                       rank_neighbors_px, &
                                       rank_neighbors_my, &
                                       rank_neighbors_py, &
                                       cartesian_communicator)
        
        time_3 = mpi_wtime()
        timers(6) = timers(6) + time_3 - time_2
        
        time_2 = mpi_wtime()
        
        Call update_domain(prev_grid, curr_grid, next_grid, Nx, Ny, C, dx, dt, Nt, omega, amplitude, it)
        
        time_3 = mpi_wtime()
        timers(3) = timers(3) + time_3 - time_2
        
        ! On applique les conditions limites
        
        time_2 = mpi_wtime()
        
        Call update_boundaries(curr_grid, local_Nx, local_Ny)
        
        time_3 = mpi_wtime()
        timers(4) = timers(4) + time_3 - time_2
        
        
        time_2 = mpi_wtime()
        
        ! On affiche des informations dans le terminal
        
        Call print_timestep_information(it)
        
        ! On écrit la grille sur le disque pour la visualiser
        
        Call output_grid(curr_grid, it, diagnostic_period, local_Nx, local_Ny, Nx, Ny, Lx, Ly)
        
        time_3 = mpi_wtime()
        timers(5) = timers(5) + time_3 - time_2
        
    end do
    
    time_2 = mpi_wtime()
    timers(2) = time_2 - time_1
    
    ! __________________________________________________________________________
    ! Bilan du temps passé dans chaque partie
    
    CALL MPI_REDUCE(timers, minimum_timers, 6, &
                    MPI_DOUBLE_PRECISION, MPI_MIN, 0, cartesian_communicator,ierror)
    CALL MPI_REDUCE(timers, maximum_timers, 6, &
                    MPI_DOUBLE_PRECISION, MPI_MAX, 0, cartesian_communicator,ierror)
    CALL MPI_REDUCE(timers, average_timers, 6, &
                    MPI_DOUBLE_PRECISION, MPI_SUM, 0, cartesian_communicator,ierror)
                            
    average_timers = average_timers / number_of_ranks
                                    
    if (rank == 0) then
        
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
        write(*,'(X,A20,3("|",X,F10.3,X),"|",X,F10.2,"%|")') &
                "Cell fantomes",minimum_timers(6),average_timers(6),&
                maximum_timers(6),percentages(6)
    endif
                    
    Call finalise_domain(prev_grid, curr_grid, next_grid)
                    
    Call MPI_FINALIZE(ierror)
                    
end program
