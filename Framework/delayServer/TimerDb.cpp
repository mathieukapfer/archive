#include "TimerDb.h"
#include "DelayServerPrio.h"
#include <string.h>
#include <stdio.h>
#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[DelayServer][TimerId]")

// ===========================
// Timers definition table
// ===========================
//    Add new timer here !
//  (order is not important)
// ===========================
#define  LED_RAMP_TICK_MS 20

// log timer related to LED only on DEBUG level
#define LOG_SPECIAL(...)                                        \
  if ((timerId == TIMER_ID_LED_RISE) ||                         \
      (timerId == TIMER_ID_LED_FALL) ||                         \
      (timerId == TIMER_ID_LED_PERIOD) ||                       \
      (timerId == TIMER_ID_LED_MAX_END)  ||                     \
      (timerId == TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER)  ||  \
      (timerId == TIMER_ID_MNG_LIFEBIT_TIMEOUT) ||              \
      (timerId == TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER_INTERNAL) || \
      (timerId == TIMER_ID__SEND_EVENT_ASYNC) ||                \
      (timerId == TIMER_ID__RECEIVE_EVENT_ASYNC) ||             \
      (timerId == TIMER_ID__EVAL_COND_ASYNC) ||                 \
      (timerId == TIMER_BMW_LED_STATUS_PERIOD) ||               \
      (timerId == TIMER_BMW_LED_STATUS_PAUSE) ||                \
      (timerId == TIMER_BMW_HMI_LIFEBIT_ERROR))                 \
    {                                                           \
      LOG_DEBUG(__VA_ARGS__);                                   \
    }                                                           \
  else                                                          \
    {                                                           \
      LOG_INFO(__VA_ARGS__);                                    \
    }                                                           \

TimerDb::TimerDefinition TimerDb::_unorderedTimerDb[TIMER_ID_MAX] = {  //
    // High priotity timer
    // ============================
      // IO
      { TIMER_ID_IO_REFRESH_CONTACTOR, 5, true, "IO_REFRESH_CONTACTOR", true },  //
      { TIMER_ID_IO_REFRESH_CPW_STATE, 5, true, "IO_REFRESH_CPW_STATE", true },  //

      // Normal priotity timer
      // ============================
      // IO
      { TIMER_ID_IO_BLINK_DO, 250, true, "IO_BLINK_DO", false },  //
      { TIMER_ID_IO_REFRESH_DI, 100, true, "IO_REFRESH_DI", false },  // to many process on this IO => put it as low priority
      { TIMER_ID_IO_REFRESH_CURRENT, 100, true, "IO_REFRESH_CURRENT", false },  // to many process on this IO => put it as low priority
      { TIMER_ID_IO_SYNCHRONOUS_BLINK_DO, 1000, true, "IO_SYNCHRONOUS_BLINK", false },  //
      // LED
      { TIMER_ID_LED_RISE, LED_RAMP_TICK_MS, true, "LED_RISE", false },  //
      { TIMER_ID_LED_FALL, LED_RAMP_TICK_MS, true, "LED_FALL", false },  //
      { TIMER_ID_LED_PERIOD, 999, true, "LED_PERIOD", false },  // duration is a default value
      { TIMER_ID_LED_MAX_END, 999, false, "LED_END", false },  // duration is a default value

      // Asynchronous action in state machine
      { TIMER_ID__SEND_EVENT_ASYNC, 0, false, "SEND_EVENT_ASYNC", false },  //
      { TIMER_ID__RECEIVE_EVENT_ASYNC, 0, false, "RECEIVE_EVENT_ASYNC", false },  //
      { TIMER_ID__EVAL_COND_ASYNC, 0, false, "EVAL_COND_ASYNC", false },  //
      { TIMER_ID__RECEIVE_RFID_STATUS_ASYNC, 0, false, "RECEIVE_RFID_STATUS_ASYNC", false },  //

      // Cpw
      { TIMER_ID_CPW_TRIGGER_CT_OPEN, 0, false, "CPW_TRIGGER_CT_OPEN", false }, //
      { TIMER_ID_CPW_TA_C_ON, 250, false, "CPW_TA_C_ON", false },  //
      { TIMER_ID_CPW_TA_C_ON_2, 250, false, "CPW_TA_C_ON_2", true },  // wait CPW1 stable before close contactor !
      // NOTE : TIMER_ID_CPW_TA_C_ON + TIMER_ID_CPW_TA_C_ON_2 should be less than 3s to be compliance to standard IEC 61851_1

      { TIMER_ID_CPW_TA_C_OFF, 60, false, "CPW_TA_C_OFF", false },  //
      { TIMER_ID_CPW_TA_C_OFF_2, 4000, false, "CPW_TA_C_OFF_2", false },  //
#ifdef _X86_
      // slower on i686: simu has to poll the CPW order at 250 ms (TIMER_ID_ForHostSimulator_CPW_Polling) in order to update EV state
      // VS immediat reaction on target
      { TIMER_ID_CPW_POLLING_COMMUNICATION_ERROR, 250, false, "CPW_POLLING_COMMUNICATION_ERROR", false },  //
#else
      { TIMER_ID_CPW_POLLING_COMMUNICATION_ERROR, 15, false, "CPW_POLLING_COMMUNICATION_ERROR", false },  //
#endif
      { TIMER_ID_CPW_CircuitBreakerDelay, 1000, false, "CPW_CircuitBreakerDelay", false },  //
      { TIMER_ID_CPW_DelayStateFBeforeChargePwmOff, 0, false, "CPW_DelayStateFBeforeChargePwmOff", false },  // duration is a default value
      { TIMER_ID_CPW_DelayStateFBeforeChargePwmOn, 0, false, "CPW_DelayStateFBeforeChargePwmOn", false },  // duration is a default value
      { TIMER_ID_CPW_EvChargeTimeout, 0, false, "CPW_EvChargeTimeout", false },  // duration is a default value
      { TIMER_ID_CPW_DelayStateFBeforeChargePwmOn2, 750, false, "CPW_DelayStateFBeforeChargePwmOn2", false }, //
      { TIMER_ID_CPW_ProgressiveMode, 5000, true, "CPW_ProgressiveMode", false },  //
      { TIMER_ID_CPW_ProgressiveMode_Polling, 5000, false, "CPW_ProgressiveMode_Polling", false },  //
      { TIMER_ID_CPW_METERING_ON, 1000, true, "CPW_METERING_ON", false },  // compute energy 4x per minutes
      { TIMER_ID_CPW_DelayedUnexptedContactorOpen, 5000, false, "CPW_DelayedUnexptedContactorOpen", false },  //
      { TIMER_ID_CPW_LowOvercurrentDelay, 0, false, "CPW_LowOvercurrentDelay", false },  //
      { TIMER_ID_CPW_MediumOvercurrentDelay, 0, false, "CPW_MediumOvercurrentDelay", false },  //
      { TIMER_ID_CPW_HighOvercurrentDelay, 0, false, "CPW_HighOvercurrentDelay", false },  //
      { TIMER_ID_CPW_OverloadDelay, 99000, false, "CPW_OvercurrentDelay", false },  // duration is a default value
      { TIMER_ID_CPW_PhaseUsage, 0, false, "CPW_PhaseUsage", false },  //
      { TIMER_ID_CPW_Set_AutonomousCondition_CpwOrder_True, 0, false, "CPW_Set_AutonomousCondition_CpwOrder_True", false },  //
      { TIMER_ID_CPW_Set_AutonomousCondition_CpwOrder_False, 0, false, "CPW_Set_AutonomousCondition_CpwOrder_False", false },  //
      { TIMER_ID_CPW_FilterUpdateImaxEv, 5000, false, "CPW_FilterUpdateImaxEv", false },  //
      { TIMER_ID_CPW_ContactorCloseHasBeenOrdered, 50, false, "ContactorCloseHasBeenOrdered", false },

      // Polling
      { TIMER_ID_RFID_POLLING, 99, true, "RFID POLLING", false },  // duration is a default value
      { TIMER_ID_RFID_POLLING_AUTOTEST_NOTIFICATION_ERROR, 10000, false, "RFID POLLING AUTOTEST NOTIFICATION ERROR", false },  //
      { TIMER_ID_POLLING_NOTIFICATION_DISABLED_MAX_PERIOD, 30000, false, "RFID notification disable max period", false },  //
      { TIMER_ID_POLLING_NOTIFICATION_DISABLED_DELAY, 5000, false, "RFID notification disable delay", false },  //

      // Mngt
      { TIMER_ID_MNG_LIFEBIT_TIMEOUT, 10000, false, "MNG_LIFEBIT_TIMEOUT", false },  //
      { TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER, 0, false, "MNG_LIFEBIT_CHECK_DELAY_SERVER", false },  //
      { TIMER_ID_MNG_LIFEBIT_CHECK_DELAY_SERVER_INTERNAL, 0, false, "MNG_LIFEBIT_CHECK_DELAY_SERVER_INTERNAL", false },  //

      // Gui
      { TIMER_ID_GUI_TEMPO_SCREEN, 5000, false, "GUI_TEMPO_SCREEN", false },  //
      { TIMER_ID_GUI_WAITING_USER, 30000, false, "GUI_WAITING_USER", false },  //
      { TIMER_ID_GUI_WAITING_SM_EVENT, 1000, false, "GUI_WAITING_SM_EVENT", false },  //

      // Plug/unplug process
      // fictive timer, never called directly, 60s=10s(TIMER_ID_WAITING_USER_ACTION_WAIT)+30s(TIMER_ID_WAITING_USER_ACTION_BUZZ)+20s(TIMER_ID_WAITING_USER_ACTION_RED))
      { TIMER_ID_PLUG_WAITING_CPW_DETECTION, 60000, false, "PLUG_WAITING_CPW_DETECTION", false },  //
      { TIMER_ID_PLUG_WAITING_CPW_DETECTION_HERMES, 30000, false, "PLUG_WAITING_CPW_DETECTION", false },  //
      { TIMER_ID_PLUG_WAITING_SHUTTER_ACTION, 30000, false, "PLUG_WAITING_SHUTTER_ACTION", false },  //
      { TIMER_ID_PLUG_LOCK_FEEDBACK_MECANISM, 500, false, "PLUG_SOCKET_LOCK_FEEDBACK_MECANISM", false },  //
      { TIMER_ID_PLUG_DISABLE_PRIMARY_LOCK, 100, false, "PLUG_SOCKET_DISABLE_PRIMARY_LOCK", false },  //
      { TIMER_ID_USER_AUTHENTIFIED_PERIOD, 5000, false, "TIMER_ID_USER_AUTHENTIFIED_PERIOD", false },  //
      { TIMER_ID_WAITING_RFID_STATUS_TIMEOUT, 10000, false, "WAITING_RFID_STATUS_TIMEOUT", false },  //
      { TIMER_ID_WAITING_USER_ACTION_WAIT, 10000, false, "WAITING_USER_ACTION_WAIT", false },  //
      { TIMER_ID_WAITING_USER_ACTION_BUZZ, 30000, false, "WAITING_USER_ACTION_BUZZ", false },  //
      { TIMER_ID_WAITING_USER_ACTION_RED, 20000, false, "WAITING_USER_ACTION_RED", false },  //
      { TIMER_ID_WAITING_USER_ACTION_SILENCE, 60000, false, "WAITING_USER_ACTION_SILENCE", false },  //
      { TIMER_ID_WAITING_USER_ACTION_SCHUCKO_ADDITIONAL_TIME, 30000, false, "WAITING_USER_ACTION_SCHUCKO_ADDITIONAL_TIME", false },  //

      // Slave state machine
      { TIMER_ID_MASTER_SLAVE_CONNECTION_TIMEOUT, 20000, false, "MASTER_SLAVE_CONNECTION_TIMEOUT", false },  //

      // Buzzer
      { TIMER_ID_BUZZER, 0, false, "BUZZER_EVENT", false },  //
      { TIMER_ID_BUZZER_RETRY, 100, false, "BUZZER_RETRY", false },  //
      // based on MasterBuzzer patternKO duration
      { TIMER_BOP_BUZZER_TIME, 700, false, "BUZZER_BOP", false },  //
      { TIMER_BOP_BUZZER_DURATION, 1000, false, "BUZZER_BOP_1sec", false },  //
      // Autonomous mode
      { TIMER_ID_WAITING_CPW_CNF_TIMEOUT, 100, false, "WAITING_CPW_CNF_TIMEOUT", false },  //

      // SM3 end fo charge
      // duration is a default value
      { TIMER_ID_SM3_END_OF_CHARGE_TIMEOUT, 999, false, "SM3_END_OF_CHARGE_TIMEOUT", false },  //
      // duration is a default value that should be replaced by the one in configuration file
      { TIMER_ID_SchukoWithNoDetection_END_OF_CHARGE_TIMEOUT, 9999, false, "SchukoWithNoDetection_END_OF_CHARGE_TIMEOUT", false },  //
      // For async reboot
      { TIMER_ID_REBOOT_COUNTDOWN, 1000, true, "REBOOT_COUNTDOWN", false },  //

      // ActiveSession table backup
      { TIMER_ID_AS_SQLITE_SAVE, 60000, true, "TIMER_ID_AS_SQLITE_SAVE", false },  //

      // Admin mode (add/remove badges)
      { TIMER_ADMIN_TIMEOUT, 30000, false, "TIMER_ADMIN_TIMEOUT", false },  //
      { TIMER_ADMIN_HERMES_TIMEOUT, 60000, false, "TIMER_ADMIN_HERMES_TIMEOUT", false },  //
      { TIMER_ADMIN_VIP_PERIOD, 3000, false, "TIMER_ADMIN_VIP_PERIOD", false },  //
      { TIMER_ADMIN_FINALIZE, 2000, false, "TIMER_ADMIN_FINALIZE", false },  //

      // OCPP meterValue interval during a charge
      { TIMER_ID_METERVALUE_INTERVAL, 5000, true, "TIMER_ID_METERVALUE_INTERVAL", false },  //

      // OCPP ClockAlignedData interval during a charge
      { TIMER_ID_CLOCKALIGNEDDATA_INTERVAL, 5000, true, "TIMER_ID_CLOCKALIGNEDDATA_INTERVAL", false },  //

      // OCPP BootNotification Heartbeat
      { TIMER_ID_BootNotification_Heartbeat, 999, true, "TIMER_ID_BootNotification_Heartbeat", false },  //

      // Check end of initial metering
      { TIMER_ID_CHECK_END_INITIAL_METERING, 999, true, "TIMER_ID_CHECK_END_INITIAL_METERING", false },  //

      // OCPP Set Time from Heartbeat
//    { TIMER_ID_Set_Time_From_Heartbeat, 999, true, "TIMER_ID_Set_Time_From_Heartbeat", false},

      // OCPP Conf From Modem
      { TIMER_ID_Conf_From_Modem, 999, true, "TIMER_ID_Conf_From_Modem", false },  //

      // OCPP BootSequence Conf From Modem
      { TIMER_ID_BootSequence_Conf_From_Modem, 999, true, "TIMER_ID_BootSequence_Conf_From_Modem", false },  //

      // OCPP BootSequence Sim Card Inf From Modem
      { TIMER_ID_BootSequence_Sim_Card_Inf_From_Modem, 999, true, "TIMER_ID_BootSequence_Sim_Card_Inf_From_Modem", false },  //

      // OCPP BootSequence BootNotification
      { TIMER_ID_BootSequence_BootNotification, 999, false, "TIMER_ID_BootSequence_BootNotification", false },  //

      // OCPP Send Request
      { TIMER_ID_SendRequest, 999, false, "TIMER_ID_SendRequest", false },  //

      // OCPP check DI8
      { TIMER_ID_CHECK_DI8, 999, false, "TIMER_ID_CHECK_DI8", false },  //

      // OCPP do reboot
      { TIMER_ID_DO_REBOOT, 999, false, "TIMER_ID_DO_REBOOT", false },  //

      // BMW polling power meters
      { TIMER_BMW_PMPOLLING, 100, true, "POWER_METER_POLLING", false },  //
      { TIMER_BMW_LOG_60MINUTES, 60000, true, "LOG_60_SECONDS", false },  // activate every 60 seconds  (one minute)
      { TIMER_BMW_LOGTRANSACTION, 60000, true, "LOGTRANSACTION", false },  //TIMER_BMW_LOGTRANSACTION
      // BMW virtual time
      { TIMER_BMW_VIRTUALTIME, 60000, true, "VIRTUALTIME", false },  //
      // LED STATUS
      { TIMER_BMW_LED_STATUS_PERIOD, 1400, true, "BMW_PERIOD_LED_STATUS", false },  //
      { TIMER_BMW_LED_STATUS_PAUSE, 0, false, "BMW_PAUSE_LED_STATUS", false },  //
      // PROXIMITY DETECTION
      { TIMER_BMW_PRESENCE_PERIOD_CHECK, 0, false, "BMW_PRESENCE_DETECTION", false },  //
      { TIMER_BMW_PROXIMITY_FILTER_DELAY, 0, false, "BMW_PROXIMITY_DETECTION_FILTER", false },  //
      { TIMER_BMW_HMI_LIFEBIT_ERROR, 30000, false, "BMW_HMI_LIFEBIT_ERROR", false },  //

      // BMW Remote enerergy management lifebit
      { TIMER_BMW_REMOTE_EM_LIFEBIT, 10000, false, "TIMER_BMW_REMOTE_EM_LIFEBIT", false },  //

      // Baseline energy management
      { TIMER_ID_EnergySharing_UpdateSetPointPeriod, 9999, true, "EnergySharing_UpdateSetPointPeriod", false },  //
      { TIMER_ID_EnergySharing_SendDelayedSetPoint, 10000, false, "EnergySharing_SendDelayedSetPoint", false },  //

      // check ntpd connection
      { TIMER_ID_NTPD_CHECK_STATUS, 300000, true, "TIMER_ID_NTPD_CHECK_STATUS", false },  //
      // Error management
      { TIMER_ID_ConnectionToMasterCheck, 10000, true, "CheckConnectionToMaster", false },  //
      { TIMER_ID_ConnectionToSlaveCheck, 30000, true, "CheckConnectionToSlave", false },  //
      { TIMER_ID_ConnectionErrorToPowermeter, 10000, false, "ConnectionErrorToPowermeter", false },  //

      // Manual back to factory
      { TIMER_ID_BackToFactory_Order, 10000, false, "ManualBackToFactory_Order", false },  //
      { TIMER_ID_BackToFactory_Confirm, 5000, false, "ManualBackToFactory_Confirm", false },  //
      { TIMER_ID_BackToFactory_End, 3000, false, "ManualBackToFactory_Ended", false },  //

      // Manual error display
      { TIMER_ID_ErrorDisplay_Order, 2000, false, "ErrorDisplay_Order", false },  //
      { TIMER_ID_ErrorDisplay_Pulse, 500, false, "ErrorDisplay_Pulse", false },  //
      { TIMER_ID_ErrorDisplay_WaitNextError, 2000, false, "ErrorDisplay_WaitNextError", false },  //

      // Host simulator test, Polling relays
      { TIMER_ID_ForHostSimulator_RELAY1_Polling, 50, true, "HostSim_Relays1_Polling", false },  //
      { TIMER_ID_ForHostSimulator_RELAY4_Polling, 250, true, "HostSim_Relays4_Polling", false },  //
      { TIMER_ID_ForHostSimulator_CPW_Polling, 100, true, "HostSim_CPW_Polling", false },  //
      { TIMER_ID_ForHostSimulator_RetryTimout, 30000, false, "HostSim_Retry_Timout", false },  //
      { TIMER_ID_ForHostSimulator_PLC_LifeBitPolling, 7000, true, "HostSim_PLC_LifeBitPolling", false },  //
      { TIMER_ID_ForHostSimulator_ThirdParty_LifeBitPolling, 7000, true, "HostSim_ThirdParty_LifeBitPolling", false },  //

      //Connectivity
      { TIMER_ID_CONNECTIVITY_REPORT_PERIOD_INTERVAL, 60000, true, "TIMER_ID_CONNECTIVITY_REPORT_PERIOD_INTERVAL", false },  //
      { TIMER_ID_CONNECTIVITY_ERROR_NOTIFICATION_RETRY, 300000, false, "TIMER_ID_CONNECTIVITY_ERROR_NOTIFICATION_RETRY", false },  //
      { TIMER_ID_CONNECTIVITY_CDR_NOTIFICATION_RETRY, 3600000, false, "TIMER_ID_CONNECTIVITY_CDR_NOTIFICATION_RETRY", false },  //
      { TIMER_ID_CONNECTIVITY_CDR_OTHER_NOTIFICATION_RETRY, 300000, false, "TIMER_ID_CONNECTIVITY_CDR_NOTIFICATION_RETRY", false },  //

      { TIMER_ID_REMOTE_AUTHENT_LIFEBIT, 10000, false, "TIMER_ID_REMOTE_AUTHENT_LIFEBIT", false },  //
      { TIMER_ID_REMOTE_EM_LIFEBIT, 10000, false, "TIMER_ID_REMOTE_EM_LIFEBIT", false },  //

      { TIMER_ID_STATION_OVERLOAD_PROTECTION, 9999, true, "TIMER_ID_STATION_OVERLOAD_PROTECTION", false },  //
      { TIMER_ID_STATION_OVERCURRENT_PROTECTION, 9999, true, "TIMER_ID_STATION_OVERCURRENT_PROTECTION", false },  //
      { TIMER_ID_STATION_OVERCURRENT_HighOvercurrentDelay, 9999, false, "TIMER_ID_STATION_OVERCURRENT_HighOvercurrentDelay", false },  //
      { TIMER_ID_STATION_OVERCURRENT_MediumOvercurrentDelay, 9999, false, "TIMER_ID_STATION_OVERCURRENT_HighOvercurrentDelay", false },  //
      { TIMER_ID_STATION_OVERCURRENT_LowOvercurrentDelay, 9999, false, "TIMER_ID_STATION_OVERCURRENT_HighOvercurrentDelay", false },  //

//timers for maintenance city mode
      { TIMER_ID_MAINTENANCE_WAIT_PUSHBUTTON, 30000, false, "TIMER_ID_MAINTENANCE_WAIT_PUSHBUTTON", false },  //
      { TIMER_ID_MAINTENANCE_WAIT_OPENSHUTTER, 30000, false, "TIMER_ID_MAINTENANCE_WAIT_OPENSHUTTER", false },  //
      // HERMES contactor
      { TIMER_ID_BasicConctactorAsynchNotification, 0, false, "BasicConctactorAsynchNotification", false },
      // Hermes Hmi Init Sequence
      { TIMER_ID_HERMES_HMI_INIT_LIGHT_ON, 1000, false, "HERMES_HMI_INIT_LIGHT_ON", false },  //
      { TIMER_ID_HERMES_HMI_INIT_LIGHT_OFF, 500, false, "HERMES_HMI_INIT_LIGHT_OFF", false },  //
      // Hermes Lock and capacitor timers
      { TIMER_ID_HERMES_ENERGY_RESERVE_ERROR, 2500, false, "HERMES_ENERGY_RESERVE_ERROR", false },  //
      { TIMER_ID_HERMES_LOCK_STATE_CHECK, 1200, false, "HERMES_LOCK_STATE_CHECK", false },  //
      { TIMER_ID_HERMES_UNLOCK_STATE_CHECK, 1200, false, "HERMES_UNLOCK_STATE_CHECK", false }, //
      { TIMER_ID_HERMES_WAIT_UNLOCK_ON_FAIL_LOCK, 1000, false, "HERMES_WAIT_UNLOCK_ON_FAIL_LOCK", false },  //
      { TIMER_ID_HERMES_SOCKET_AUTOTEST_DELAY, 1500, false, "HERMES_SOCKET_AUTOTEST_DELAY", false },

      //Hermes authenticaiton failed
      { TIMER_ID_HERMES_HMI_AUTH_KO, 5000, false, "HERMES_HMI_AUTH_KO", false },  //
      // Hermes time between two sample of delayedCharge input (cyclic)
      { TIMER_ID_HERMES_DELAYED_CHARGE_SAMPLES_INTERVAL, 175, true, "HERMES_DELAYED_CHARGE_SAMPLES_INTERVAL", false },  //
      { TIMER_ID_HANDSHAKE_POSTPONE_LOW_POWER, 30000, false, "HERMES_HANDSHAKE_POSTPONE_LOW_POWER", false },  //
      { TIMER_ID_HERMES_DELAYED_CHARGE_DISPLAYFORCECHARGE, 4000, false, "HERMES_DELAYED_FORCE_CHARGE", false },  //
      { TIMER_ID_HERMES_AUTHENTICATION_BIP, 1000, true, "HERMES_AUTHENTICATION_BIP", false },  //
      { TIMER_ID_HERMES_STOP_CHARGE_RFID, 30000, false, "HERMES_STOP_CHARGE_RFID", false },  //

      { TIMER_ID_RESERVATION_EXPIRATION, 60000, true, "RESERVATION_EXPIRATION", false },  //
      { TIMER_ID_HERMES_BACKTOFACTORY, 5000, false, "BACKTOFACTORY", false },  //
      { TIMER_ID_HERMES_MULTIPLEBIPS, 500, true, "MULTPILE BIP", false },  //
#ifdef _X86_
      // should be > 2*TIMER_ID_ForHostSimulator_CPW_Polling
      { TIMER_ID_WAIT_RESTORE_SESSION, 200, false, "WAIT_RESTORE_SESSION", false },  //
#else
      { TIMER_ID_WAIT_RESTORE_SESSION, 100, false, "WAIT_RESTORE_SESSION", false },  //
#endif
      { TIMER_ID_WAIT_SM_EVENT_RESTORE_RESTART_MAIN_PLUG_PROCESS_PLUG1, 1000, false, "RESTART_CHARGE_PLUG1", false},
      { TIMER_ID_WAIT_SM_EVENT_RESTORE_RESTART_MAIN_PLUG_PROCESS_PLUG2, 1000, false, "RESTART_CHARGE_PLUG2", false},
      { TIMER_ID_WAIT_SM_EVENT_RESTORE_START_UNPLUG_PROCESS_PLUG1, 1000, false, "RESTART_UNPLUG1", false}, // Go to Unplug process
      { TIMER_ID_WAIT_SM_EVENT_RESTORE_START_UNPLUG_PROCESS_PLUG2, 1000, false, "RESTART_UNPLUG2", false}, // Go to Unplug process


      { TIMER_ID_OCPP_MINIMUM_STATUS_DURATION, 2000, false, "OCPP_MINIMUM_STATUS_DURATION", false},
      { TIMER_ID_OCPP_AUTHORIZE_TIMEOUT, 10000, false, "TIMER_ID_OCPP_AUTHORIZE_TIMEOUT", false},
      { TIMER_ID_SMART_CHARGING_INTERVAL, 60000, true, "TIMER_ID_SMART_CHARGING_INTERVAL", false},
      { TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_OK, 1000, false, "TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_OK", false},
      { TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_SLAVE_OK, 2000, false, "TIMER_ID_WAIT_SM_EVENT_COMM_VLAN_SLAVE_OK", false},

#ifdef _X86_
      { TIMER_ID_GENERIC_CLOCK, 999, true, "TIMER_ID_GENERIC_CLOCK", false},
#endif
    };

// ordered table
TimerDb::TimerDefinition * TimerDb::_timerDb[TIMER_ID_MAX];

// singleton
TimerDb * TimerDb::_globalInstance = 0;

TimerDb * TimerDb::inst() {
  if (!_globalInstance) _globalInstance = new TimerDb();

  return _globalInstance;
}

void TimerDb::deleteInst() {
  if (_globalInstance) {
    delete _globalInstance;
  }

}

TimerDb::TimerDb() {

  LOG_ENTER();

  // clean table index
  memset(_timerDb, 0, sizeof(TimerDefinition *) * TIMER_ID_MAX);

  // build table index
  for (int tableIndex = 0; tableIndex < TIMER_ID_MAX; tableIndex++) {
    TimerDefinition *timer = &_unorderedTimerDb[tableIndex];
    _timerDb[timer->timerId] = timer;
  }

}

void TimerDb::Display() {
  log("\n Timer table definition \n");
  log("index = {name, id, duration, isPeriodic} \n ==================================== \n");
  log(" --------------- unordred list --------------------- \n");
  for (int tableIndex = 0; tableIndex < TIMER_ID_MAX; tableIndex++) {
    TimerDefinition *timer = &_unorderedTimerDb[tableIndex];
    log("timer[%d] = { %-40s, %4d, %8u, %s} \n", timer->timerId, timer->name, timer->timerId, timer->duration, timer->isPeriodic ? "true" : "false");
  }
  log(" ---------------  ordred list --------------------- \n");
  for (int tableIndex = 0; tableIndex < TIMER_ID_MAX; tableIndex++) {
    TimerDefinition *timer = _timerDb[tableIndex];
    log("timer[%d] = { %-40s, %4d, %8u, %s} \n", timer->timerId, timer->name, timer->timerId, timer->duration, timer->isPeriodic ? "true" : "false");
  }
}

void TimerDb::StartTimer(TimerId timerId, TimerEventI *obj, int duration) {
  LOG_ENTER("%d\n", timerId);
  TimerDefinition *timerDef = _timerDb[timerId];
  LOG_SPECIAL("Start timer %2d=%s with %d ms as %s (prio:%s)\n", timerDef->timerId, timerDef->name, (duration?duration:timerDef->duration), timerDef->isPeriodic?"periodic":"single shot", timerDef->isPrio?"yes":"no");
  if (timerDef->isPrio == false) {
    DelayServer::inst()->registerTimer((int) timerDef->timerId, (duration ? duration : timerDef->duration), timerDef->isPeriodic, obj);
  } else {
    DelayServerPrio::inst()->registerTimer((int) timerDef->timerId, (duration ? duration : timerDef->duration), timerDef->isPeriodic, obj);
  }
}

void TimerDb::StopTimer(TimerId timerId, TimerEventI *obj) {
  LOG_ENTER("%d\n", timerId);
  TimerDefinition *timerDef = _timerDb[timerId];
  LOG_DEBUG("Stop timer %2d=%s", timerId, timerDef->name);
  if (timerDef->isPrio == false) {
    DelayServer::inst()->unregisterTimer((int) timerId, obj);
  } else {
    DelayServerPrio::inst()->unregisterTimer((int) timerId, obj);
  }
}

char * TimerDb::GetName(int timerId) {
  return (_timerDb[timerId])->name;
}

bool TimerDb::isTimerActif(int timer, TimerEventI *obj) {
  const TimerDefinition *timerDef = _timerDb[timer];
  bool ret;
  if (timerDef->isPrio == false) {
    ret = DelayServer::inst()->isTimerActif(timer, obj);
  } else {
    ret = DelayServerPrio::inst()->isTimerActif(timer, obj);
  }
  return ret;
}

void TimerDb::ChangeTimer(TimerId timerId, int duration) {
  LOG_ENTER("%d\n", timerId);
  if (timerId < TIMER_ID_MAX) {
    _timerDb[timerId]->duration = duration;
  } else {
    LOG_ERROR("Unknow timer id: %d (action canceled)", timerId);
  }
}

int TimerDb::GetDuration(TimerId timerId) {
  int ret = 0;
  if (timerId < TIMER_ID_MAX) {
    ret = _timerDb[timerId]->duration;
  } else {
    LOG_ERROR("Unknow timer id: %d (action canceled)", timerId);
  }
  return ret;
}
