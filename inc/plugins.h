#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <statemachine.h>
#include <QMap>

namespace hfsmexec
{
    class InvokeStatePlugin
    {
        public:
            virtual ~InvokeStatePlugin() {}
            virtual bool invoke() = 0;
            virtual bool cancel() = 0;

            const QString& getPluginId() const;

        protected:
            QString pluginId;
    };

    class InvokeStatePluginLoader
    {
        public:
            InvokeStatePluginLoader();

            bool load(QMap<QString, InvokeStatePlugin*>& plugins);
            bool load(const QString& path, QMap<QString, InvokeStatePlugin*>& plugins);
    };

    class InvokeStatePluginLoaderTest
    {
        public:
            InvokeStatePluginLoaderTest();
    };
}

Q_DECLARE_INTERFACE(hfsmexec::InvokeStatePlugin, "InvokeStatePlugin")

#endif
