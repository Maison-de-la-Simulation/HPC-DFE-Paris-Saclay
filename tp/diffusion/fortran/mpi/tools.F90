! ______________________________________________________________________________
!
! Module tools
!
! Description: ce module contient un certain nombre de fonctions "outils"
! pour le code tel qu'un parser pour les données entrées en ligne de commande
! ______________________________________________________________________________
module tools

    implicit none

contains

    ! __________________________________________________________________________
    !
    ! Get the command line arguments
    ! __________________________________________________________________________
    subroutine get_arguments(ranks_per_direction, Nt, Nx,  &
                             Ny, diagnostic_period, D, Lx, Ly)

        implicit none

        integer, dimension(2), intent(inout)    :: ranks_per_direction    ! Répartition des rangs dans chaque direction
        integer, intent(inout)                  :: Nt                     ! Nombre d'itération
        integer, intent(inout)                  :: Nx, Ny                 ! Nombre de points dans chaque direction pour tout le domaine
        integer, intent(inout)                  :: diagnostic_period      ! Période en itération entre chaque sortie
        real(8), intent(inout)                  :: D                      ! Coéfficient de diffusion
        real(8), intent(inout)                  :: Lx, Ly                 ! Taille du domaine dans la direction x puis y (grandeur arbitraire)

        integer           :: number_of_arguments
        integer           :: i
        character(len=32) :: arg1, arg2, arg3

        number_of_arguments = COMMAND_ARGUMENT_COUNT()

        if (number_of_arguments > 0) then

            i = 1
            do while (i<number_of_arguments)
                Call GET_COMMAND_ARGUMENT(i,arg1)

                if (trim(arg1).eq."-ranksxy") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    Call GET_COMMAND_ARGUMENT(i+2,arg3)
                    read(arg2,*) ranks_per_direction(1)
                    read(arg3,*) ranks_per_direction(2)
                    i = i + 3
                end if

                if (trim(arg1).eq."-Nt") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) Nt
                    i = i + 2
                end if

                if (trim(arg1).eq."-Nxy") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    Call GET_COMMAND_ARGUMENT(i+2,arg3)
                    read(arg2,*) Nx
                    read(arg3,*) Ny
                    i = i + 3
                end if

                if (trim(arg1).eq."-diags") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) diagnostic_period
                    i = i + 2
                end if

                if (trim(arg1).eq."-D") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    read(arg2,*) D
                    i = i + 2
                end if

                if (trim(arg1).eq."-Lxy") then
                    Call GET_COMMAND_ARGUMENT(i+1,arg2)
                    Call GET_COMMAND_ARGUMENT(i+2,arg3)
                    read(arg2,*) Lx
                    read(arg3,*) Ly
                    i = i + 3
                end if

            end do

        end if

    end subroutine

end module
