#pragma once

struct DGram_Vehicle
{
    short m_gram_header;      // ����ͷ 0xAAA
    short m_gram_id;          // ���ı�ʶ 0xAF1
    short m_vehicle_id;       // ����ID 0x00A ~ 0x00C
    short m_speed;            // �ٶ�
    int m_steering;           // ������ת��
    short m_gear;             // ��λ
    short m_acc_pedal;        // ����̤��λ�� 1 - 100
    short m_brake_pedal;      // ɲ��̤��λ�� 1 - 100
    short m_rpm;              // ������ת��
    short m_hopper_state;     // ��״̬��0�½���1������2�޶���
    short m_hopper_angle;     // ���Ƕ�
    short m_vehicle_state;    // 0:����; 1:����
    short m_ctrl_sign;        // 0:�Զ�; 1:ң��
    short m_ctrl_info;        // ������Ϣ 1:������������ӹ�; 2:�������� ...
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
    short m_gram_header;      // ����ͷ 0xA1A
    short m_gram_id;          // ���ı�ʶ 0xA11  
    short m_vehicle_id;       // ����ID 0x00A ~ 0x00C
    short m_rc_acc_pedal;     // ���ſ���
    short m_rc_brake_pedal;   // ɲ��̤�忪��
    short m_rc_hopper;        // ң�ض�״̬��0�½���1������2�޶���
    short m_rc_gear;          // ң�ص�λ R:-1, N:0, D:1
    short m_rc_speed;         // ң���ٶ�
    short m_ctrl_sign;        // ң��״̬, 1: ң��; 0: �Զ�
    short m_unused;
    int m_rc_steer;           // ң�ط�����ת��
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