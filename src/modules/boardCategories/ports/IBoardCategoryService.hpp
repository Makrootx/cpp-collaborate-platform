#pragma once

#include "modules/boardCategories/adapters/dto/BoardCategoryDto.hpp"
#include <vector>

class IBoardCategoryService
{
public:
    virtual ~IBoardCategoryService() = default;
    virtual BoardCategoryResultDto create_board_category(const BoardCategoryCreateDto &dto) = 0;
    virtual BoardCategoryResultDto get_board_category_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<BoardCategoryResultDto> get_all_board_categories_query(const std::vector<std::string> &relations) = 0;
    virtual BoardCategoryResultDto update_board_category(const BoardCategoryUpdateDto &dto) = 0;
    virtual void delete_board_category_by_id(long int id) = 0;

    virtual BoardCategoryResultSecureDto get_board_category_secured_by_id_query(long int id, const std::vector<std::string> &relations) = 0;
    virtual std::vector<BoardCategoryResultSecureDto> get_all_board_categories_query_secured(const std::vector<std::string> &relations) = 0;
    virtual BoardCategoryResultSecureUpdateDto update_board_category_secured(const BoardCategoryUpdateDto &dto) = 0;
};
