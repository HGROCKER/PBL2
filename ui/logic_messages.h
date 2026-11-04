#pragma once
#ifndef PLB2_LOGIC_MESSAGES_H
#define PLB2_LOGIC_MESSAGES_H

#include <QString>
#include "../core/application_logic.h"

inline QString logicErrorMessage(plb2::LogicError error) {
    switch (error) {
    case plb2::LogicError::InvalidName:
        return "Tên không được để trống.";
    case plb2::LogicError::NotFound:
        return "Không tìm thấy dữ liệu cần cập nhật hoặc xóa.";
    case plb2::LogicError::DuplicateId:
        return "Không thể thêm dữ liệu vì ID đã tồn tại.";
    case plb2::LogicError::InvalidSchedule:
        return "Lịch thi đấu không hợp lệ: mỗi cặp đội chỉ đá một lần trên sân nhà.";
    case plb2::LogicError::CannotDelete:
        return "Không thể giải tán đội bóng. Đội có thể đang được sử dụng trong trận đấu.";
    case plb2::LogicError::None:
        break;
    }
    return "Thao tác không thể hoàn tất.";
}

#endif
