#pragma once

struct DGram_Vehicle
{
    short m_gram_header;      // 报文头 0xAAA
    short m_gram_id;          // 报文标识 0xAF1
    short m_vehicle_id;       // 车辆ID 0x00A ~ 0x00C
    short m_speed;            // 速度
    int m_steering;           // 方向盘转角
    short m_gear;             // 档位
    short m_acc_pedal;        // 油门踏板位置 1 - 100
    short m_brake_pedal;      // 刹车踏板位置 1 - 100
    short m_rpm;              // 发动机转速
    short m_hopper_state;     // 斗状态：0下降，1上升，2无动作
    short m_hopper_angle;     // 斗角度
    short m_vehicle_state;    // 0:正常; 1:故障
    short m_ctrl_sign;        // 0:自动; 1:遥控
    short m_ctrl_info;        // 控制信息 1:车辆主动请求接管; 2:车辆故障 ...
    short m_release_feedback;
    int m_lat_error;
    int m_lon_error;
    int m_angle_error;
    int m_path_x;
    int m_path_y;
    short m_path_heading;
    short m_path_id;
    short m_basket_fill;
    short m_unused0;

    DGram_Vehicle(short _vehicle_id) : m_gram_header(0xAAA),
        m_gram_id(0xAF1),
        m_vehicle_id(_vehicle_id),
        m_vehicle_state(0),
        m_ctrl_sign(0),
        m_ctrl_info(0),
        m_hopper_state(2),
        m_basket_fill(0)
    {

    }
};


struct DGram_RemoteControl
{
    short m_gram_header;      // 报文头 0xA1A
    short m_gram_id;          // 报文标识 0xA11  
    short m_vehicle_id;       // 车辆ID 0x00A ~ 0x00C
    short m_rc_acc_pedal;     // 油门开度
    short m_rc_brake_pedal;   // 刹车踏板开度
    short m_rc_hopper;        // 遥控斗状态，0下降，1上升，2无动作
    short m_rc_gear;          // 遥控档位 R:-1, N:0, D:1
    short m_rc_speed;         // 遥控速度
    short m_ctrl_sign;        // 遥控状态, 1: 遥控; 0: 自动
    short m_unused;
    int m_rc_steer;           // 遥控方向盘转角
    short m_arrive_port;
    short m_unuse0;
    short m_unuse1;
    short m_unuse2;

    DGram_RemoteControl() {}

    DGram_RemoteControl(short _vehicle_id) : m_gram_header(0xAAA),
        m_gram_id(0xAF2),
        m_vehicle_id(_vehicle_id),
        m_rc_acc_pedal(0),
        m_rc_brake_pedal(0),
        m_rc_steer(0),
        m_rc_hopper(2),
        m_ctrl_sign(0)
    {

    }
};