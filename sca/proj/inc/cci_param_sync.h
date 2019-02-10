
#ifndef CCI_PARAM_SYNC_H
#define CCI_PARAM_SYNC_H

#include <cci_configuration>
#include <vector>
#include <sstream>

/**
 *  @class  cci_param_sync
 *  @brief  This module registers post write callbacks on the owner's parameters
 *          in order to update an owner cci_parameter directly when another is modified
 */
SC_MODULE(cci_param_sync) {
 public:
  SC_HAS_PROCESS(cci_param_sync);

  /**
   *  @fn     cci_param_sync(sc_core::sc_module_name _name, std::vector<cci::cci_param_handle> BaseParamList)
   *  @brief  The class constructor
   *  @param  _name The name of the class
   *  @param  BaseParamList The list of CCI parameter handles
   *  @return void
   */
  cci_param_sync(sc_core::sc_module_name _name,
                        std::vector<cci::cci_param_handle> BaseParamList)
      // Define an originator in order to get hold of the default broker
      : m_broker(cci::cci_get_broker())
  {
    // Copy the list of selected base parameters to a local std::vector
    returnBaseParamList = BaseParamList;

    for (unsigned int i = 1; i < returnBaseParamList.size(); i++) {
      synchValues(returnBaseParamList[0], returnBaseParamList[i]);
    }
  }

  /**
   *  @fn     void typed_post_write_callback(const cci::cci_param_write_event<int> & ev)
   *  @brief  Post Callback function to sync ongoing written parameter value with synced_handle value
   *  @return void
   */
  void untyped_post_write_callback(const cci::cci_param_write_event<> & ev ,
                                   cci::cci_param_handle synced_handle)
  {
    synced_handle.set_cci_value(ev.new_value);
  }

  /**
   *  @fn     void synchValues(cci::cci_param_handle _param_handle_1, cci::cci_param_handle _param_handle_2)
   *  @brief  Function for synchronizing the values of cci_parameter of OWNER
   *          modules via the PARAM_VALUE_SYNC
   *  @param  _param_handle_1 The first parameter to be synced
   *  @param  _param_handle_2 The second parameter to be synced
   *  @return void
   */
  void synchValues(cci::cci_param_handle _param_handle_1,
                   cci::cci_param_handle _param_handle_2) {
    // In order to synchronize even the default values of the owner modules,
    // use cci_base_param of one parameter as reference, write the same value
    // to the other pararmeter's cci_base_param using JSON
    _param_handle_2.set_cci_value(_param_handle_1.get_cci_value());

    post_write_cb_vec.push_back(_param_handle_1.register_post_write_callback(
            sc_bind(&cci_param_sync::untyped_post_write_callback,
                    this, sc_unnamed::_1, _param_handle_2)));

    post_write_cb_vec.push_back(_param_handle_2.register_post_write_callback(
            sc_bind(&cci_param_sync::untyped_post_write_callback,
                    this, sc_unnamed::_1, _param_handle_1)));
  }

 private:
  cci::cci_broker_handle m_broker; ///< Declaring a CCI configuration broker handle
  std::vector<cci::cci_callback_untyped_handle> post_write_cb_vec; ///< Callback Adaptor Objects
  std::vector<cci::cci_param_handle> returnBaseParamList; ///< std::vector storing the searched owner parameters references to CCI parameter handles
};
// cci_param_sync

#endif  // CCI_PARAM_SYNC_H

