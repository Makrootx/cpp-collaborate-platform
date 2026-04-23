#include "modules/boards/adapters/BoardService.hpp"
#include "modules/boards/domain/Board.hpp"
#include "modules/boards/domain/BoardWrappers.hpp"
#include <spdlog/spdlog.h>

BoardResultDto BoardService::create_board(const BoardCreateDto &dto)
{
    Board board(dto.title, dto.description);
    auto id = repo_->create(board);
    auto created = repo_->find_by_id_query(id, {});
    if (!created.has_value())
        throw std::runtime_error("Board not found after creation");
    return BoardResultDto::from_domain(created.value());
}

BoardResultDto BoardService::get_board_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto board_opt = repo_->find_by_id_query(id, relations);
    if (!board_opt.has_value())
    {
        spdlog::error("Board with id {} not found", id);
        throw std::runtime_error("Board not found");
    }
    return BoardResultDto::from_domain(board_opt.value());
}

std::vector<BoardResultDto> BoardService::get_all_boards_query(const std::vector<std::string> &relations)
{
    auto boards = repo_->get_all_query(relations);
    std::vector<BoardResultDto> results;
    for (const auto &board : boards)
        results.push_back(BoardResultDto::from_domain(board));
    return results;
}

BoardResultDto BoardService::update_board(const BoardUpdateDto &dto)
{
    auto board_opt = repo_->find_by_id(dto.id);
    if (!board_opt.has_value())
    {
        spdlog::error("Board with id {} not found for update", dto.id);
        throw std::runtime_error("Board not found");
    }
    auto board = board_opt.value();
    if (dto.title.has_value())
        board.set_title(dto.title.value());
    if (dto.description.has_value())
        board.set_description(dto.description.value());
    repo_->update(board);
    return BoardResultDto::from_domain(board);
}

void BoardService::delete_board_by_id(long int id)
{
    repo_->delete_by_id(id);
}

BoardResultSecureDto BoardService::get_board_secured_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto board_opt = repo_->find_by_id_query(id, relations);
    if (!board_opt.has_value())
    {
        spdlog::error("Board with id {} not found", id);
        throw std::runtime_error("Board not found");
    }
    auto board = board_opt.value();
    auto board_automative = AutomativeBoard{board, space_id_};
    auto board_secured = SecureBoard{board_automative, required_permission_};
    auto result = BoardResultSecureDto::from_domain(board_secured);
    board_automative.flush_domain_actions();
    return result.merge_with_plain(BoardResultDto::from_domain(board));
}

std::vector<BoardResultSecureDto> BoardService::get_all_boards_query_secured(const std::vector<std::string> &relations)
{
    auto boards = repo_->get_all_query(relations);
    std::vector<BoardResultSecureDto> results;
    for (auto &board : boards)
    {
        auto board_automative = AutomativeBoard{board, space_id_};
        auto board_secured = SecureBoard{board_automative, required_permission_};
        auto result = BoardResultSecureDto::from_domain(board_secured);
        board_automative.flush_domain_actions();
        results.push_back(result.merge_with_plain(BoardResultDto::from_domain(board)));
    }
    return results;
}

BoardResultSecureUpdateDto BoardService::update_board_secured(const BoardUpdateDto &dto)
{
    BoardResultSecureUpdateDto secure_update_dto;
    auto board_opt = repo_->find_by_id(dto.id);
    if (!board_opt.has_value())
    {
        spdlog::error("Failed to update board: board with id {} not found", dto.id);
        throw std::runtime_error("Board not found");
    }
    auto board_plain = board_opt.value();
    auto board = SecureBoard{AutomativeBoard{board_plain, space_id_}, required_permission_};

    if (dto.title.has_value())
        secure_update_dto.title = board.set_title(dto.title.value());
    if (dto.description.has_value())
        secure_update_dto.description = board.set_description(dto.description.value());

    return secure_update_dto;
}
