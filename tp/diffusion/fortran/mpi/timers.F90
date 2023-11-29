! ______________________________________________________________________________
!
! Module timers
!
! Description: this module contains the functions to record time for
! the different sections of the code
! ______________________________________________________________________________

module timers

    use parameters
    use mpi
    implicit none

    integer                                      :: number_of_timers
    real(8), dimension(10)                       :: temporary_timers
    real(8), dimension(10)                       :: local_timers
    real(8), dimension(10)                       :: minimum_timers
    real(8), dimension(10)                       :: maximum_timers
    real(8), dimension(10)                       :: average_timers
    real(8), dimension(10)                       :: percentages
    character(len=20), dimension(10)             :: timer_names

    contains

    ! __________________________________________________________________________
    ! Timer initialization
    !
    ! By default `number_of_timers` = 2
    ! Compulsory timers are:
    ! - initialization (id = 1)
    ! - Main loop (id = 2)
    ! __________________________________________________________________________
    subroutine timers_initialize
        implicit none
        integer             :: n
        integer             :: i

        ! Minimum number of timers = 2 for compulsory timers
        number_of_timers = 2

        ! Initialization
        local_timers = 0

        ! Name of compulsory timers
        timer_names(1) = 'Initialization'
        timer_names(2) = 'Main loop'

        ! Default names for the others
        do i=3,10
            timer_names(i) = "Unamed timer"
        enddo

    end subroutine

    ! __________________________________________________________________________
    ! Set name of timer of identification number `id`
    ! Input:
    ! - name (character): name of the timer
    ! - id (integer)    : timer id in the array
    ! __________________________________________________________________________
    subroutine timers_set_name(name,id)
        implicit none
        integer, intent(in) :: id
        character(len=*), intent(in) :: name

        timer_names(id) = name

        if (number_of_timers < id) then
            number_of_timers = id
        end if

    end subroutine

    ! __________________________________________________________________________
    ! Mark the time when called for the timer `id`
    ! Input:
    ! - id (integer): timer id in the array
    ! __________________________________________________________________________
    subroutine timers_mark(id)
        implicit none
        integer, intent(in) :: id

        temporary_timers(id) = MPI_Wtime()

    end subroutine

    ! __________________________________________________________________________
    ! Add the time from the last marker to timer `id`
    ! Input:
    ! - id (integer): timer id in the array
    ! __________________________________________________________________________
    subroutine timers_add(id)
        implicit none
        integer, intent(in) :: id

        local_timers(id) = local_timers(id) + MPI_Wtime() - temporary_timers(id)

    end subroutine

    ! __________________________________________________________________________
    ! Finalization of the timers
    ! Computation of the min, max and average among all MPI processes
    ! __________________________________________________________________________
    subroutine timers_finalize()
        implicit none

        integer :: ierror

        CALL MPI_REDUCE(local_timers, minimum_timers, number_of_timers, &
                        MPI_DOUBLE_PRECISION, MPI_MIN, 0, cartesian_communicator,ierror)
        CALL MPI_REDUCE(local_timers, maximum_timers, number_of_timers, &
                        MPI_DOUBLE_PRECISION, MPI_MAX, 0, cartesian_communicator,ierror)
        CALL MPI_REDUCE(local_timers, average_timers, number_of_timers, &
                        MPI_DOUBLE_PRECISION, MPI_SUM, 0, cartesian_communicator,ierror)

        average_timers = average_timers / number_of_ranks

        percentages(:) = average_timers(:) / (average_timers(1) + average_timers(2)) * 100.

    end subroutine

    ! __________________________________________________________________________
    !
    ! Show the timers in the terminal
    ! __________________________________________________________________________
    subroutine timers_show()
        implicit none
        integer :: i

        if (rank.EQ.0) then

            print*, ' '
            print('(X,"_________________________________________________________________________")')
            print*,''
            print*,'        Timers                                                         '
            print('(X,"_________________________________________________________________________")')
            print('(X,"                    |            |            |            |            |")')

            write(*,'(X,A20,4("|",X,A10,X),"|")') "Code part     ","min (s)","mean (s)","max (s)","percentage"
            print('(X,"____________________|____________|____________|____________|____________|")')
            print('(X,"                    |            |            |            |            |")')

            ! Compulsory timers
            write(*,'(X,A20,3("|",X,F10.2,X),"|",X,F10.2,"%|")') &
            timer_names(1),minimum_timers(1),average_timers(1),&
            maximum_timers(1),percentages(1)
            write(*,'(X,A20,3("|",X,F10.2,X),"|",X,F10.2,"%|")') &
            timer_names(2),minimum_timers(2),average_timers(2),&
            maximum_timers(2),percentages(2)

            print('(X,"____________________|____________|____________|____________|____________|")')
            print('(X,"                    |            |            |            |            |")')
            ! Additional timers
            if (number_of_timers > 2) then
                Do i=3,number_of_timers
                  write(*,'(X,A20,3("|",X,F10.2,X),"|",X,F10.2,"%|")') &
                  timer_names(i),minimum_timers(i),average_timers(i),&
                  maximum_timers(i),percentages(i)
                enddo
            endif

            print('(X,"____________________|____________|____________|____________|____________|")')

        endif

    end subroutine

end module
