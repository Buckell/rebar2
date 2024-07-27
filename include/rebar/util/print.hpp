//
// Created by maxng on 27/07/2024.
//

#ifndef PRINT_HPP
#define PRINT_HPP

namespace rebar {

    template <typename ...t_args>
    void print(t_args const & ...a_args) {
        (std::cout << ... << a_args);
    }

    template <typename ...t_args>
    void println(t_args const & ...a_args) {
        (std::cout << ... << a_args) << '\n';
    }

}

#endif //PRINT_HPP

