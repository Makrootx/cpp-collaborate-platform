#pragma once

#include "modules/boards/ports/IBoardService.hpp"
#include "modules/boards/ports/IBoardRepo.hpp"
#include "shared/adapters/permission_core/PermissionStruct.hpp"
#include <memory>

class BoardService : public IBoardService
{
    long int space_id_ = -1;
    GroupPermission required_permission_;
    std::shared_ptr<IBoardRepo> repo_;

public:
    explicit BoardService(long int space_id, std::shared_ptr<IBoardRepo> repo, GroupPermission required_permission = {})
        : space_id_(space_id), required_permission_(std::move(required_permission)), repo_(std::move(repo)) {}

    BoardResultDto create_board(const BoardCreateDto &dto) override;
    BoardResultDto get_board_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<BoardResultDto> get_all_boards_query(const std::vector<std::string> &relations) override;
    BoardResultDto update_board(const BoardUpdateDto &dto) override;
    void delete_board_by_id(long int id) override;

    BoardResultSecureDto get_board_secured_by_id_query(long int id, const std::vector<std::string> &relations) override;
    std::vector<BoardResultSecureDto> get_all_boards_query_secured(const std::vector<std::string> &relations) override;
    BoardResultSecureUpdateDto update_board_secured(const BoardUpdateDto &dto) override;
};
