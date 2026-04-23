#include "modules/boardCategories/adapters/BoardCategoryService.hpp"
#include "modules/boardCategories/domain/BoardCategory.hpp"
#include "modules/boardCategories/domain/BoardCategoryWrappers.hpp"
#include "modules/boards/domain/Board.hpp"
#include <spdlog/spdlog.h>

BoardCategoryResultDto BoardCategoryService::create_board_category(const BoardCategoryCreateDto &dto)
{
    auto board_opt = board_repo_->find_by_id(dto.board_id);
    if (!board_opt.has_value())
    {
        spdlog::error("Failed to create board category: board with id {} not found", dto.board_id);
        throw std::runtime_error("Parent board not found");
    }
    BoardCategory cat(dto.title);
    auto id = repo_->create(cat);
    board_repo_->link_category(dto.board_id, id);
    auto created = repo_->find_by_id_query(id, {});
    if (!created.has_value())
        throw std::runtime_error("BoardCategory not found after creation");
    return BoardCategoryResultDto::from_domain(created.value());
}

BoardCategoryResultDto BoardCategoryService::get_board_category_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto cat_opt = repo_->find_by_id_query(id, relations);
    if (!cat_opt.has_value())
    {
        spdlog::error("BoardCategory with id {} not found", id);
        throw std::runtime_error("BoardCategory not found");
    }
    return BoardCategoryResultDto::from_domain(cat_opt.value());
}

std::vector<BoardCategoryResultDto> BoardCategoryService::get_all_board_categories_query(const std::vector<std::string> &relations)
{
    auto cats = repo_->get_all_query(relations);
    std::vector<BoardCategoryResultDto> results;
    for (const auto &cat : cats)
        results.push_back(BoardCategoryResultDto::from_domain(cat));
    return results;
}

BoardCategoryResultDto BoardCategoryService::update_board_category(const BoardCategoryUpdateDto &dto)
{
    auto cat_opt = repo_->find_by_id(dto.id);
    if (!cat_opt.has_value())
    {
        spdlog::error("BoardCategory with id {} not found for update", dto.id);
        throw std::runtime_error("BoardCategory not found");
    }
    auto cat = cat_opt.value();
    if (dto.title.has_value())
        cat.set_title(dto.title.value());
    repo_->update(cat);
    return BoardCategoryResultDto::from_domain(cat);
}

void BoardCategoryService::delete_board_category_by_id(long int id)
{
    repo_->delete_by_id(id);
}

BoardCategoryResultSecureDto BoardCategoryService::get_board_category_secured_by_id_query(long int id, const std::vector<std::string> &relations)
{
    auto cat_opt = repo_->find_by_id_query(id, relations);
    if (!cat_opt.has_value())
    {
        spdlog::error("BoardCategory with id {} not found", id);
        throw std::runtime_error("BoardCategory not found");
    }
    auto cat = cat_opt.value();
    auto cat_automative = AutomativeBoardCategory{cat, space_id_};
    auto cat_secured = SecureBoardCategory{cat_automative, required_permission_};
    auto result = BoardCategoryResultSecureDto::from_domain(cat_secured);
    cat_automative.flush_domain_actions();
    return result.merge_with_plain(BoardCategoryResultDto::from_domain(cat));
}

std::vector<BoardCategoryResultSecureDto> BoardCategoryService::get_all_board_categories_query_secured(const std::vector<std::string> &relations)
{
    auto cats = repo_->get_all_query(relations);
    std::vector<BoardCategoryResultSecureDto> results;
    for (auto &cat : cats)
    {
        auto cat_automative = AutomativeBoardCategory{cat, space_id_};
        auto cat_secured = SecureBoardCategory{cat_automative, required_permission_};
        auto result = BoardCategoryResultSecureDto::from_domain(cat_secured);
        cat_automative.flush_domain_actions();
        results.push_back(result.merge_with_plain(BoardCategoryResultDto::from_domain(cat)));
    }
    return results;
}

BoardCategoryResultSecureUpdateDto BoardCategoryService::update_board_category_secured(const BoardCategoryUpdateDto &dto)
{
    BoardCategoryResultSecureUpdateDto secure_update_dto;
    auto cat_opt = repo_->find_by_id(dto.id);
    if (!cat_opt.has_value())
    {
        spdlog::error("Failed to update board category: id {} not found", dto.id);
        throw std::runtime_error("BoardCategory not found");
    }
    auto cat_plain = cat_opt.value();
    auto cat = SecureBoardCategory{AutomativeBoardCategory{cat_plain, space_id_}, required_permission_};

    if (dto.title.has_value())
        secure_update_dto.title = cat.set_title(dto.title.value());

    return secure_update_dto;
}
