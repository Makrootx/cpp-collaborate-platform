#pragma once

#include "modules/boardCategories/ports/IBoardCategoryService.hpp"
#include "modules/boardCategories/ports/IBoardCategoryRepo.hpp"
#include "modules/boards/ports/IBoardRepo.hpp"
#include "shared/adapters/permission_core/PermissionStruct.hpp"
#include <memory>

class BoardCategoryService : public IBoardCategoryService
{
    long int space_id_ = -1;
    GroupPermission required_permission_;
    std::shared_ptr<IBoardCategoryRepo> repo_;
    std::shared_ptr<IBoardRepo> board_repo_;

public:
    explicit BoardCategoryService(long int space_id, std::shared_ptr<IBoardCategoryRepo> repo,
                                  std::shared_ptr<IBoardRepo> board_repo, GroupPermission required_permission = {})
        : space_id_(space_id), required_permission_(std::move(required_permission)),
          repo_(std::move(repo)), board_repo_(std::move(board_repo)) {}

    BoardCategoryResultDto create_board_category(const BoardCategoryCreateDto &dto) override;
    BoardCategoryResultDto get_board_category_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<BoardCategoryResultDto> get_all_board_categories_query(const std::vector<std::string> &relations) override;
    BoardCategoryResultDto update_board_category(const BoardCategoryUpdateDto &dto) override;
    void delete_board_category_by_id(long int id) override;

    BoardCategoryResultSecureDto get_board_category_secured_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<BoardCategoryResultSecureDto> get_all_board_categories_query_secured(const std::vector<std::string> &relations) override;
    BoardCategoryResultSecureUpdateDto update_board_category_secured(const BoardCategoryUpdateDto &dto) override;
};
