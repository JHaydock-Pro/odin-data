/*
 * FileWriterController.h
 *
 *  Created on: 27 May 2016
 *      Author: gnx91527
 */

#ifndef TOOLS_FILEWRITER_FILEWRITERCONTROLLER_H_
#define TOOLS_FILEWRITER_FILEWRITERCONTROLLER_H_

#include <boost/shared_ptr.hpp>
#include <log4cxx/logger.h>

#include "FileWriterPlugin.h"
#include "IpcReactor.h"
#include "IpcChannel.h"
#include "SharedMemoryController.h"
#include "SharedBufferManager.h"
#include "ClassLoader.h"

namespace filewriter
{

 /**
  * The FileWriterController class has overall responsibility for management of the
  * core classes and plugins present within the file writer application.  This class
  * maintains the SharedMemoryController and SharedMemoryParser classes.  The class
  * also manages the control IpcChannel, and accepts configuration IpcMessages.  The
  * class provides an interface for loading plugins, connecting the plugins together
  * into chains and for configuring the plugins (from the control channel).
  *
  * The class uses an IpcReactor to manage connections and status updates.
  */
  class FileWriterController : public IFrameCallback, public boost::enable_shared_from_this<FileWriterController>
  {
  public:
    FileWriterController();
    virtual ~FileWriterController();
    void handleCtrlChannel();
    void configure(OdinData::IpcMessage& config, OdinData::IpcMessage& reply);
    void configurePlugin(OdinData::IpcMessage& config, OdinData::IpcMessage& reply);
    void loadPlugin(const std::string& index, const std::string& name, const std::string& library);
    void connectPlugin(const std::string& index, const std::string& connectTo);
    void disconnectPlugin(const std::string& index, const std::string& disconnectFrom);
    void run();
    void waitForShutdown();
  private:
    /** Configuration constant to shutdown the file writer process **/
    static const std::string CONFIG_SHUTDOWN;

    /** Configuration constant for retrieving the status **/
    static const std::string CONFIG_STATUS;

    /** Configuration constant for name of shared memory storage **/
    static const std::string CONFIG_FR_SHARED_MEMORY;
    /** Configuration constant for connection string for frame release **/
    static const std::string CONFIG_FR_RELEASE;
    /** Configuration constant for connection string for frame ready **/
    static const std::string CONFIG_FR_READY;
    /** Configuration constant for executing setup of shared memory interface **/
    static const std::string CONFIG_FR_SETUP;

    /** Configuration constant for control socket endpoint **/
    static const std::string CONFIG_CTRL_ENDPOINT;

    /** Configuration constant for plugin related items **/
    static const std::string CONFIG_PLUGIN;
    /** Configuration constant for listing loaded plugins **/
    static const std::string CONFIG_PLUGIN_LIST;
    /** Configuration constant for loading a plugin **/
    static const std::string CONFIG_PLUGIN_LOAD;
    /** Configuration constant for connecting plugins **/
    static const std::string CONFIG_PLUGIN_CONNECT;
    /** Configuration constant for disconnecting plugins **/
    static const std::string CONFIG_PLUGIN_DISCONNECT;
    /** Configuration constant for a plugin name **/
    static const std::string CONFIG_PLUGIN_NAME;
    /** Configuration constant for a plugin index **/
    static const std::string CONFIG_PLUGIN_INDEX;
    /** Configuration constant for a plugin external library **/
    static const std::string CONFIG_PLUGIN_LIBRARY;
    /** Configuration constant for setting up a plugin connection **/
    static const std::string CONFIG_PLUGIN_CONNECTION;

    void setupFrameReceiverInterface(const std::string& sharedMemName,
                                     const std::string& frPublisherString,
                                     const std::string& frSubscriberString);
    void closeFrameReceiverInterface();
    void setupControlInterface(const std::string& ctrlEndpointString);
    void closeControlInterface();
    void runIpcService(void);
    void tickTimer(void);
    void callback(boost::shared_ptr<Frame> frame);

    /** Pointer to the logging facility */
    log4cxx::LoggerPtr                                          logger_;
    /** Pointer to the shared memory controller instance for this process */
    boost::shared_ptr<SharedMemoryController>                   sharedMemController_;
    /** Pointer to the shared buffer manager instance for this process */
    boost::shared_ptr<OdinData::SharedBufferManager>            sharedBufferManager_;
    /** Map of plugins loaded, indexed by plugin index */
    std::map<std::string, boost::shared_ptr<FileWriterPlugin> > plugins_;
    /** Condition for exiting this file writing process */
    boost::condition_variable                                   exitCondition_;
    /** Frames per dataset */
    int                                                         datasetSize;
    /** Total frames processed */
    int                                                         totalFrames;
    /** Master frame specifier - Frame to include in count of total frames processed */
    std::string                                                 masterFrame;
    /** Mutex used for locking the exitCondition */
    boost::mutex                                                exitMutex_;
    /** Used to check for Ipc tick timer termination */
    bool                                                        runThread_;
    /** Is the main thread running */
    bool                                                        threadRunning_;
    /** Did an error occur during the thread initialisation */
    bool                                                        threadInitError_;
    /** Have we sent sent the shutdown command to the plugins */
    bool                                                        pluginShutdownSent;
    /** Main thread used for control message handling */
    boost::thread                                               ctrlThread_;
    /** Store for any messages occurring during thread initialisation */
    std::string                                                 threadInitMsg_;
    /** Pointer to the IpcReactor for incoming frame handling */
    boost::shared_ptr<OdinData::IpcReactor>                reactor_;
    /** IpcChannel for control messages */
    OdinData::IpcChannel                                   ctrlChannel_;
  };

} /* namespace filewriter */

#endif /* TOOLS_FILEWRITER_FILEWRITERCONTROLLER_H_ */