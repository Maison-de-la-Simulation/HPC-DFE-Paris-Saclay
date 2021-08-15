! ______________________________________________________________________________
!
! Implémentation de l'équation de diffusion 2D
!
! Méthode : différence finie
! Conditions limites : Dirichlet
!
! ______________________________________________________________________________

program diffusion

    use parameters
    use parallelism
    use physics
    use diagnostics
    use timers
    use tools

    implicit none

    ! __________________________________________________________________________
    ! Initialisation du parallélisme

    ! Initialisation de MPI
    call initialize_mpi(number_of_ranks, rank)

    ! Définition des paramètres de simulation par défaut
    ranks_per_direction = (/2 , 2/)           ! Nombre de rangs dans chaque dimension
    Lx                  = 1.                  ! Taille du domaine dans la direction x
    Ly                  = 1.                  ! Taille du domaine dans la direction y
    Nx                  = 150                 ! Nombre de points sur la grille dans la direction x
    Ny                  = 100                 ! Nombre de points sur la grille dans la direction y
    D                   = 0.01                ! Coefficient de diffusion
    Nt                  = 5000                ! Nombre d'itérations temporelles
    alpha               = 0.5                 ! Facteur sur le pas en temps calculé avec la CFL
    print_period        = 1000                ! Période de sortie à l'écran des itérations
    diagnostic_period   = 100                 ! Période en nombre d'itération entre chaque sortie de fichier

    ! On récupère les arguments rentrés par ligne de commande
    ! qui viendront écraser ceux par défaut
    Call get_arguments(ranks_per_direction, Nt, Nx, Ny, &
                       diagnostic_period, D, Lx, Ly)

    ! Création de la topologie cartésienne MPI pour la décomposition de domaine
    call create_cartesian_topology(rank,                &
                                 ranks_per_direction, &
                                 rank_coordinates,    &
                                 rank_neighbors_mx,   &
                                 rank_neighbors_px,   &
                                 rank_neighbors_my,   &
                                 rank_neighbors_py,   &
                                 cartesian_communicator)

    ! Initialisation des timers pour mesurer le temps
    call timers_initialize
    call timers_set_name("MPI",3)

    ! __________________________________________________________________________
    ! Initialisation du domaine

    ! Initialisation des paramètres physiques et du premier domaine
    call timers_mark(1)
    call initialize_domain(Nx, Ny,                    &
                         Lx, Ly,                    &
                         d, alpha,                  &
                         nt,                        &
                         rank,                      &
                         number_of_ranks,           &
                         rank_coordinates,          &
                         ranks_per_direction,       &
                         cartesian_communicator,    &
                         dt,                        &
                         first_print,               &
                         dx, dy,                    &
                         cx, cy,                    &
                         local_Lx, local_Ly,        &
                         local_x_min, local_x_max,  &
                         local_y_min, local_y_max,  &
                         local_Nx, local_Ny, U, Up)

    call timers_add(1)

    call output_diags(0)

    ! __________________________________________________________________________
    ! Coeur du calcul

    ! Boucle en temps
    call timers_mark(2)
    do it=1,Nt

        call update_domain(U, Up, local_Nx, local_Ny, cx, cy)

        call exchange_gost_cells(U, local_Nx, local_Ny,    &
                               rank_neighbors_mx, &
                               rank_neighbors_px, &
                               rank_neighbors_my, &
                               rank_neighbors_py, &
                               cartesian_communicator)

        Call update_boundaries(U,                       &
                            local_x_min, local_x_max, &
                            local_y_min, local_y_max, &
                            local_Nx, local_Ny,                   &
                            Lx, Ly)

        call print_timestep_information(it)

        call output_diags(it)

    enddo
    call timers_add(2)

    ! __________________________________________________________________________
    ! Timers

    call timers_finalize()

    call timers_show()

    ! __________________________________________________________________________
    ! Nettoyage et fin du programme

    ! Finalisation des paramètres du domaine
    call finalise_domain(U, Up)

    ! Finalisation de MPI
    call finalize_mpi()

end program
