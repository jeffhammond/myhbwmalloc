module fortran_hbwmalloc
      use, intrinsic :: iso_c_binding

      interface

        subroutine myhbwmalloc_init() bind(C,name="myhbwmalloc_init")
        end subroutine myhbwmalloc_init

        subroutine myhbwmalloc_final() bind(C,name="myhbwmalloc_final")
        end subroutine myhbwmalloc_final

        type(C_ptr) function hbw_malloc(size) bind(C,name="hbw_malloc")
          import C_ptr, C_size_t
          integer(C_size_t), value, intent(in) :: size
        end function hbw_malloc

        type(C_ptr) function hbw_calloc(nmemb, size) bind(C,name="hbw_calloc")
          import C_ptr, C_size_t
          integer(C_size_t), value, intent(in) :: size, nmemb
        end function hbw_calloc

        type(C_ptr) function hbw_realloc(ptr, size) bind(C,name="hbw_realloc")
          import C_ptr, C_size_t
          integer(C_size_t), value, intent(in) :: size
          type(C_ptr), value, intent(in) :: ptr
        end function hbw_realloc

        subroutine hbw_free(ptr) bind(C,name="hbw_free")
          import C_ptr
          type(C_ptr), value, intent(in) :: ptr
        end subroutine hbw_free

        integer(C_int) function hbw_check_available() bind(C,name="hbw_check_available")
          import C_int
        end function hbw_check_available


     end interface
end module

