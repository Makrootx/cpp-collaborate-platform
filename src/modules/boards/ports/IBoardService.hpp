#pragma once

#include "modules/boards/adapters/dto/BoardDto.hpp"
#include <vector>

class IBoardService
{
public:
    virtual ~IBoardService() = default;
    virtual BoardResultDto create_board(const BoardCreateDto &dto) = 0;
    virtual BoardResultDto get_board_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<BoardResultDto> get_all_boards_query(const std::vector<std::string> &relations) = 0;
    virtual BoardResultDto update_board(const BoardUpdateDto &dto) = 0;
    virtual void delete_board_by_id(long int id) = 0;

    virtual BoardResultSecureDto get_board_secured_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<BoardResultSecureDto> get_all_boards_query_secured(const std::vector<std::string> &relations) = 0;
    virtual BoardResultSecureUpdateDto update_board_secured(const BoardUpdateDto &dto) = 0;
};
