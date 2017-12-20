#ifndef TIMER_DB_INCLUDED
#define TIMER_DB_INCLUDED

#include "TimerEventI.h"
#include "stdint.h"

typedef enum {
  // Asynchronous event
  TIMER_ID__SEND_EVENT_ASYNC,
  TIMER_ID__RECEIVE_EVENT_ASYNC,
  TIMER_ID__EVAL_COND_ASYNC,
  TIMER_ID__RECEIVE_RFID_STATUS_ASYNC,

  // Cpw
  TIMER_ID_CPW_TRIGGER_CT_OPEN,
  TIMER_ID_CPW_TA_C_ON,
  TIMER_ID_CPW_TA_C_ON_2,
  TIMER_ID_CPW_TA_C_OFF,
  TIMER_ID_CPW_TA_C_OFF_2,
  TIMER_ID_CPW_POLLING_COMMUNICATION_ERROR,  // IO Polling
  TIMER_ID_CPW_CircuitBreakerDelay,
  TIMER_ID_CPW_DelayStateFBeforeChargePwmOff,
  TIMER_ID_CPW_DelayStateFBeforeChargePwmOn,
  TIMER_ID_CPW_DelayStateFBeforeChargePwmOn2,
  TIMER_ID_CPW_EvChargeTimeout,
  TIMER_ID_CPW_ProgressiveMode,
  TIMER_ID_CPW_ProgressiveMode_Polling,
  TIMER_ID_CPW_METERING_ON,
  TIMER_ID_CPW_DelayedUnexptedContactorOpen,
  TIMER_ID_CPW_LowOvercurrentDelay,
  TIMER_ID_CPW_MediumOvercurrentDelay,
  TIMER_ID_CPW_HighOvercurrentDelay,
  TIMER_ID_CPW_OverloadDelay,
  TIMER_ID_CPW_PhaseUsage,
  TIMER_ID_CPW_Set_AutonomousCondition_CpwOrder_True,
  TIMER_ID_CPW_Set_AutonomousCondition_CpwOrder_False,
  TIMER_ID_CPW_FilterUpdateImaxEv,
  TIMER_ID_CPW_ContactorCloseHasBeenOrdered,

  // RFID Polling
  TIMER_ID_RFID_POLLING,
  TIMER_ID_RFID_POLLING_AUTOTEST_NOTIFICATION_ERROR,
  TIMER_ID_POLLING_NOTIFICATION_DISABLED_MAX_PERIOD,
  TIMER_ID_POLLING_NOTIFICATION_DISABLED_DELAY,

  // IO
  TIMER_ID_IO_BLINK_DO,
  TIMER_ID_IO_REFRESH_DI,
  TIMER_ID_IO_REFRESH_CURRENT,
  TIMER_ID_IO_REFRESH_CONTACTOR,
  TIMER_ID_IO_REFRESH_CPW_STATE,
  TIMER_ID_IO_SYNCHRONOUS_BLINK_DO,

  // LED
  TIMER_ID_LED_RISE,
  TIMER_ID_LED_FALL,
  TIMER_ID_LED_PERIOD,
  TIMER_ID_LED_MAX_END,

  // Mngt
  TIMER_ID_MNG_LIFEBIT_TIMEOUT,
  TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER,
  TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER_INTERNAL,

  // Gui
  TIMER_ID_GUI_TEMPO_SCREEN,
  TIMER_ID_GUI_WAITING_USER,
  TIMER_ID_GUI_WAITING_SM_EVENT,

  // Plug/unplug process
  TIMER_ID_PLUG_WAITING_CPW_DETECTION,
  TIMER_ID_PLUG_WAITING_CPW_DETECTION_HERMES,
  TIMER_ID_PLUG_WAITING_SHUTTER_ACTION,
  TIMER_ID_PLUG_LOCK_FEEDBACK_MECANISM,
  TIMER_ID_PLUG_DISABLE_PRIMARY_LOCK,
  TIMER_ID_USER_AUTHENTIFIED_PERIOD,
  TIMER_ID_WAITING_RFID_STATUS_TIMEOUT,
  TIMER_ID_WAITING_USER_ACTION_WAIT,
  TIMER_ID_WAITING_USER_ACTION_BUZZ,
  TIMER_ID_WAITING_USER_ACTION_RED,
  TIMER_ID_WAITING_USER_ACTION_SILENCE,
  TIMER_ID_WAITING_USER_ACTION_SCHUCKO_ADDITIONAL_TIME,
  // Slave state machine
  TIMER_ID_MASTER_SLAVE_CONNECTION_TIMEOUT,

  // Buzzer
  TIMER_ID_BUZZER,
  TIMER_ID_BUZZER_RETRY,
  TIMER_BOP_BUZZER_TIME,
  TIMER_BOP_BUZZER_DURATION,

  // Autonomous mode
  TIMER_ID_WAITING_CPW_CNF_TIMEOUT,

  // Cpw end of charge
  TIMER_ID_SM3_END_OF_CHARGE_TIMEOUT,
  TIMER_ID_SchukoWithNoDetection_END_OF_CHARGE_TIMEOUT,

  // For async reboot
  TIMER_ID_REBOOT_COUNTDOWN,

  // ActiveSession table backup
  TIMER_ID_AS_SQLITE_SAVE,

  // Admin mode (add/remove badges)
  TIMER_ADMIN_TIMEOUT,
  TIMER_ADMIN_HERMES_TIMEOUT,
  TIMER_ADMIN_VIP_PERIOD,
  TIMER_ADMIN_FINALIZE,

  // BMW polling timer
  TIMER_BMW_PMPOLLING,
  TIMER_BMW_LOG_60MINUTES,  // activate every 60 seconds  (one minute)
  TIMER_BMW_LOGTRANSACTION,  // Log transaction timer
  // BMW virtual time
  TIMER_BMW_VIRTUALTIME,
  // added for BMW status Led period
  TIMER_BMW_LED_STATUS_PERIOD,
  TIMER_BMW_LED_STATUS_PAUSE,
  // added for BMW presence manager
  TIMER_BMW_PRESENCE_PERIOD_CHECK,
  TIMER_BMW_PROXIMITY_FILTER_DELAY,
  TIMER_BMW_HMI_LIFEBIT_ERROR,

  // BMW energy management
  TIMER_BMW_REMOTE_EM_LIFEBIT,
  // NTPD status
  TIMER_ID_NTPD_CHECK_STATUS,

  // Baseline energy management
  TIMER_ID_EnergySharing_UpdateSetPointPeriod,
  TIMER_ID_EnergySharing_SendDelayedSetPoint,

  // OCPP
  TIMER_ID_METERVALUE_INTERVAL,
  TIMER_ID_CLOCKALIGNEDDATA_INTERVAL,
  TIMER_ID_BootNotification_Heartbeat,
  TIMER_ID_CHECK_END_INITIAL_METERING,
  TIMER_ID_Conf_From_Modem,
  TIMER_ID_BootSequence_Conf_From_Modem,
  TIMER_ID_BootSequence_Sim_Card_Inf_From_Modem,
  TIMER_ID_BootSequence_BootNotification,
  TIMER_ID_SendRequest,
  TIMER_ID_CHECK_DI8,
  TIMER_ID_DO_REBOOT,

  // Error management
  TIMER_ID_ConnectionToMasterCheck,
  TIMER_ID_ConnectionToSlaveCheck,
  TIMER_ID_ConnectionErrorToPowermeter,

  // Manual back to factory
  TIMER_ID_BackToFactory_Order,
  TIMER_ID_BackToFactory_Confirm,
  TIMER_ID_BackToFactory_End,

  // Manual display error
  TIMER_ID_ErrorDisplay_Order,
  TIMER_ID_ErrorDisplay_WaitNextError,
  TIMER_ID_ErrorDisplay_Pulse,

  // Host simulator test, Polling relays
  TIMER_ID_ForHostSimulator_RELAY1_Polling,
  TIMER_ID_ForHostSimulator_RELAY4_Polling,
  TIMER_ID_ForHostSimulator_CPW_Polling,
  TIMER_ID_ForHostSimulator_RetryTimout,
  TIMER_ID_ForHostSimulator_PLC_LifeBitPolling,
  TIMER_ID_ForHostSimulator_ThirdParty_LifeBitPolling,

  //Connectivity
  TIMER_ID_CONNECTIVITY_REPORT_PERIOD_INTERVAL,
  TIMER_ID_CONNECTIVITY_ERROR_NOTIFICATION_RETRY,
  TIMER_ID_CONNECTIVITY_CDR_NOTIFICATION_RETRY,
  TIMER_ID_CONNECTIVITY_CDR_OTHER_NOTIFICATION_RETRY,

  // Remote Authent
  TIMER_ID_REMOTE_AUTHENT_LIFEBIT,
  TIMER_ID_REMOTE_EM_LIFEBIT,

  // Station protection
  TIMER_ID_STATION_OVERLOAD_PROTECTION,
  TIMER_ID_STATION_OVERCURRENT_PROTECTION,
  TIMER_ID_STATION_OVERCURRENT_HighOvercurrentDelay,
  TIMER_ID_STATION_OVERCURRENT_MediumOvercurrentDelay,
  TIMER_ID_STATION_OVERCURRENT_LowOvercurrentDelay,

  //admin maintenance timers
  TIMER_ID_MAINTENANCE_WAIT_PUSHBUTTON,
  TIMER_ID_MAINTENANCE_WAIT_OPENSHUTTER,

  // Hermes contactor
  TIMER_ID_BasicConctactorAsynchNotification,

  // Hermes Hmi Init Sequence
  TIMER_ID_HERMES_HMI_INIT_LIGHT_ON,
  TIMER_ID_HERMES_HMI_INIT_LIGHT_OFF,

  // Hermes Lock and capacitor timers
  TIMER_ID_HERMES_ENERGY_RESERVE_ERROR,
  TIMER_ID_HERMES_LOCK_STATE_CHECK,
  TIMER_ID_HERMES_UNLOCK_STATE_CHECK,
  TIMER_ID_HERMES_WAIT_UNLOCK_ON_FAIL_LOCK,

  //Hermes authenticaiton failed
  TIMER_ID_HERMES_HMI_AUTH_KO,
  // Hermes time between two sample of delayedCharge input
  TIMER_ID_HERMES_DELAYED_CHARGE_SAMPLES_INTERVAL,
  // duration at the beginning of a charge when we ignore the postponed event
  TIMER_ID_HANDSHAKE_POSTPONE_LOW_POWER,
  TIMER_ID_HERMES_DELAYED_CHARGE_DISPLAYFORCECHARGE,
  TIMER_ID_HERMES_AUTHENTICATION_BIP,
  TIMER_ID_HERMES_STOP_CHARGE_RFID,

  TIMER_ID_RESERVATION_EXPIRATION,  // activate every 60 seconds  (one minute)

  TIMER_ID_HERMES_BACKTOFACTORY,

  TIMER_ID_HERMES_MULTIPLEBIPS,

  //timer wait for CPW to be ready (evstate) before restore session on master behavour
  TIMER_ID_WAIT_RESTORE_SESSION,

  //restore session order from master should be delayed
  TIMER_ID_WAIT_SM_EVENT_RESTORE_RESTART_MAIN_PLUG_PROCESS_PLUG1,
  TIMER_ID_WAIT_SM_EVENT_RESTORE_RESTART_MAIN_PLUG_PROCESS_PLUG2,
  TIMER_ID_WAIT_SM_EVENT_RESTORE_START_UNPLUG_PROCESS_PLUG1,  // Go to Unplug process
  TIMER_ID_WAIT_SM_EVENT_RESTORE_START_UNPLUG_PROCESS_PLUG2,  // Go to Unplug process

  TIMER_ID_OCPP_MINIMUM_STATUS_DURATION,
  TIMER_ID_OCPP_AUTHORIZE_TIMEOUT,
  TIMER_ID_HERMES_SOCKET_AUTOTEST_DELAY,

  TIMER_ID_SMART_CHARGING_INTERVAL,

  TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_OK,
  TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_SLAVE_OK,

#ifdef _X86_
  // clock used for test engine
  TIMER_ID_GENERIC_CLOCK,
#endif

  // !!! Keep at the end !!!
  TIMER_ID_MAX

} TimerId;

class TimerDb {
 public:

  // singleton
  static TimerDb * inst();
  static void deleteInst();

  // timer actions
  void StartTimer(TimerId timerId, TimerEventI *obj, int duration = 0);
  void StopTimer(TimerId timerId, TimerEventI *obj);
  bool isTimerActif(int timer, TimerEventI *obj);

  // timer info
  int GetDuration(TimerId timerId);

  // Change the default duration of timer
  void ChangeTimer(TimerId timerId, int duration);

  // for debug purpose
  void Display();
  char *GetName(int timerId);

 private:

  // singleton
  TimerDb();
  ~TimerDb() {
  }

  static TimerDb * _globalInstance;

  // nested type for timer db

  typedef struct {
    int timerId;  // timer id
    uint32_t duration;  // in ms
    bool isPeriodic;  // true means periodic timer (restart automatically)
    char *name;  // name of timer for log
    bool isPrio;  // true means handle by high priority delay server
  } TimerDefinition;

  // timer table definition (not mandatory ordered)
  static TimerDefinition _unorderedTimerDb[TIMER_ID_MAX];

  // timer table index (ordered)
  static TimerDefinition * _timerDb[TIMER_ID_MAX];

};

#endif
