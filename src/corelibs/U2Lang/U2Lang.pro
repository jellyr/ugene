include (U2Lang.pri)

# Input
HEADERS += src/library/BaseActorCategories.h \
           src/library/BaseAttributes.h \
           src/library/BasePorts.h \
           src/library/BaseSlots.h \
           src/library/BaseTypes.h \
           src/library/LastReadyScheduler.h \
           src/library/LocalDomain.h \
           src/model/ActorPrototypeRegistry.h \
           src/model/Aliasing.h \
           src/model/Attribute.h \
           src/model/AttributeRelation.h \
           src/model/Configuration.h \
           src/model/ConfigurationEditor.h \
           src/model/ConfigurationValidator.h \
           src/model/Datatype.h \
           src/model/DatatypeConversion.h \
           src/model/DbiDataHandler.h \
           src/model/DbiDataStorage.h \
           src/model/Descriptor.h \
           src/model/ExternalToolCfg.h \
           src/model/GrouperOutSlot.h \
           src/model/GrouperSlotAttribute.h \
           src/model/IncludedProtoFactory.h \
           src/model/IntegralBus.h \
           src/model/IntegralBusModel.h \
           src/model/IntegralBusType.h \
           src/model/Marker.h \
           src/model/MarkerAttribute.h \
           src/model/Peer.h \
           src/model/Port.h \
           src/model/QDConstraint.h \
           src/model/QDScheme.h \
           src/model/QueryDesignerRegistry.h \
           src/model/ReadDbObjActorPrototype.h \
           src/model/RunFileSystem.h \
           src/model/Schema.h \
           src/model/SchemaActorsRegistry.h \
           src/model/SchemaConfig.h \
           src/model/WorkflowContext.h \
           src/model/WorkflowEnv.h \
           src/model/WorkflowManager.h \
           src/model/WorkflowScriptEngine.h \
           src/model/WorkflowTransport.h \
           src/run/WorkflowMonitor.h \
           src/support/AttributeInfo.h \
           src/support/BaseBreakpointHitCounter.h \
           src/support/BaseNGSWorker.h \
           src/support/BreakpointConditionChecker.h \
           src/support/BreakpointEqualHitCounter.h \
           src/support/BreakpointGreaterOrEqualHitCounter.h \
           src/support/BreakpointMultipleHitCounter.h \
           src/support/CoreLibConstants.h \
           src/support/ElapsedTimeUpdater.h \
           src/support/IntegralBusUtils.h \
           src/support/MapDatatypeEditor.h \
           src/support/MarkerUtils.h \
           src/support/NoFailTaskWrapper.h \
           src/support/SchemaSerializer.h \
           src/support/ScriptWorkerSerializer.h \
           src/support/SimpleWorkflowTask.h \
           src/support/SupportClass.h \
           src/support/SupportStructures.h \
           src/support/WorkflowBreakpoint.h \
           src/support/WorkflowBreakpointSharedInfo.h \
           src/support/WorkflowDebugMessageParser.h \
           src/support/WorkflowDebugStatus.h \
           src/support/WorkflowEnvImpl.h \
           src/support/WorkflowInvestigationData.h \
           src/support/WorkflowIOTasks.h \
           src/support/WorkflowRunTask.h \
           src/support/WorkflowSettings.h \
           src/support/WorkflowUtils.h \
           src/tasks/ReadDocumentTaskFactory.h \
           src/tasks/SchemaEstimationTask.h \
           src/tasks/WorkflowTasksRegistry.h \
           src/library/script/ActorContext.h \
           src/library/script/DbiClassPrototype.h \
           src/library/script/ScriptEngineUtils.h \
           src/library/script/ScriptLibrary.h \
           src/library/script/SequencePrototype.h \
           src/model/actor/Actor.h \
           src/model/actor/ActorConfigurationEditor.h \
           src/model/actor/ActorDocument.h \
           src/model/actor/ActorPrototype.h \
           src/model/actor/ActorScriptValidator.h \
           src/model/actor/ActorValidator.h \
           src/model/url_attribute/Dataset.h \
           src/model/url_attribute/SharedDbUrlUtils.h \
           src/model/url_attribute/URLAttribute.h \
           src/model/url_attribute/URLContainer.h \
           src/model/wizard/ElementSelectorWidget.h \
           src/model/wizard/IdMapping.h \
           src/model/wizard/PortMapping.h \
           src/model/wizard/SelectorValue.h \
           src/model/wizard/SlotMapping.h \
           src/model/wizard/Variable.h \
           src/model/wizard/Wizard.h \
           src/model/wizard/WizardPage.h \
           src/model/wizard/WizardWidget.h \
           src/model/wizard/WizardWidgetVisitor.h \
           src/support/external_script/ScriptableScheduler.h \
           src/support/external_script/ScriptContext.h \
           src/support/external_script/WorkflowRunSerializedSchemeTask.h \
           src/support/serialize/Constants.h \
           src/support/serialize/HRSchemaSerializer.h \
           src/support/serialize/HRVisualSerializer.h \
           src/support/serialize/HRWizardSerializer.h \
           src/support/serialize/OldUWL.h \
           src/support/serialize/Tokenizer.h \
           src/support/serialize/Utils.h
SOURCES += src/library/BaseActorCategories.cpp \
           src/library/BaseAttributes.cpp \
           src/library/BasePorts.cpp \
           src/library/BaseSlots.cpp \
           src/library/BaseTypes.cpp \
           src/library/LastReadyScheduler.cpp \
           src/library/LocalDomain.cpp \
           src/model/ActorPrototypeRegistry.cpp \
           src/model/Aliasing.cpp \
           src/model/Attribute.cpp \
           src/model/AttributeRelation.cpp \
           src/model/Configuration.cpp \
           src/model/ConfigurationEditor.cpp \
           src/model/Datatype.cpp \
           src/model/DbiDataHandler.cpp \
           src/model/DbiDataStorage.cpp \
           src/model/Descriptor.cpp \
           src/model/ExternalToolCfg.cpp \
           src/model/GrouperOutSlot.cpp \
           src/model/GrouperSlotAttribute.cpp \
           src/model/IncludedProtoFactory.cpp \
           src/model/IntegralBus.cpp \
           src/model/IntegralBusModel.cpp \
           src/model/IntegralBusType.cpp \
           src/model/Marker.cpp \
           src/model/MarkerAttribute.cpp \
           src/model/Port.cpp \
           src/model/QDConstraint.cpp \
           src/model/QDScheme.cpp \
           src/model/QueryDesignerRegistry.cpp \
           src/model/ReadDbObjActorPrototype.cpp \
           src/model/RunFileSystem.cpp \
           src/model/Schema.cpp \
           src/model/SchemaActorsRegistry.cpp \
           src/model/WorkflowContext.cpp \
           src/model/WorkflowEnv.cpp \
           src/model/WorkflowScriptEngine.cpp \
           src/model/WorkflowTransport.cpp \
           src/run/WorkflowMonitor.cpp \
           src/support/AttributeInfo.cpp \
           src/support/BaseBreakpointHitCounter.cpp \
           src/support/BaseNGSWorker.cpp \
           src/support/BreakpointConditionChecker.cpp \
           src/support/BreakpointEqualHitCounter.cpp \
           src/support/BreakpointGreaterOrEqualHitCounter.cpp \
           src/support/BreakpointMultipleHitCounter.cpp \
           src/support/CoreLibConstants.cpp \
           src/support/ElapsedTimeUpdater.cpp \
           src/support/IntegralBusUtils.cpp \
           src/support/MapDatatypeEditor.cpp \
           src/support/MarkerUtils.cpp \
           src/support/NoFailTaskWrapper.cpp \
           src/support/SchemaSerializer.cpp \
           src/support/ScriptWorkerSerializer.cpp \
           src/support/SimpleWorkflowTask.cpp \
           src/support/SupportClass.cpp \
           src/support/SupportStructures.cpp \
           src/support/WorkflowBreakpoint.cpp \
           src/support/WorkflowBreakpointSharedInfo.cpp \
           src/support/WorkflowDebugMessageParser.cpp \
           src/support/WorkflowDebugStatus.cpp \
           src/support/WorkflowEnvImpl.cpp \
           src/support/WorkflowIOTasks.cpp \
           src/support/WorkflowRunTask.cpp \
           src/support/WorkflowSettings.cpp \
           src/support/WorkflowUtils.cpp \
           src/tasks/ReadDocumentTaskFactory.cpp \
           src/tasks/SchemaEstimationTask.cpp \
           src/tasks/WorkflowTasksRegistry.cpp \
           src/library/script/ActorContext.cpp \
           src/library/script/DbiClassPrototype.cpp \
           src/library/script/ScriptEngineUtils.cpp \
           src/library/script/ScriptLibrary.cpp \
           src/library/script/SequencePrototype.cpp \
           src/model/actor/Actor.cpp \
           src/model/actor/ActorConfigurationEditor.cpp \
           src/model/actor/ActorPrototype.cpp \
           src/model/actor/ActorScriptValidator.cpp \
           src/model/actor/ActorValidator.cpp \
           src/model/url_attribute/Dataset.cpp \
           src/model/url_attribute/SharedDbUrlUtils.cpp \
           src/model/url_attribute/URLAttribute.cpp \
           src/model/url_attribute/URLContainer.cpp \
           src/model/wizard/ElementSelectorWidget.cpp \
           src/model/wizard/IdMapping.cpp \
           src/model/wizard/PortMapping.cpp \
           src/model/wizard/SelectorValue.cpp \
           src/model/wizard/SlotMapping.cpp \
           src/model/wizard/Variable.cpp \
           src/model/wizard/Wizard.cpp \
           src/model/wizard/WizardPage.cpp \
           src/model/wizard/WizardWidget.cpp \
           src/support/external_script/ScriptableScheduler.cpp \
           src/support/external_script/ScriptContext.cpp \
           src/support/external_script/WorkflowRunSerializedSchemeTask.cpp \
           src/support/serialize/Constants.cpp \
           src/support/serialize/HRSchemaSerializer.cpp \
           src/support/serialize/HRVisualSerializer.cpp \
           src/support/serialize/HRWizardSerializer.cpp \
           src/support/serialize/OldUWL.cpp \
           src/support/serialize/Tokenizer.cpp \
           src/support/serialize/Utils.cpp
RESOURCES += U2Lang.qrc
TRANSLATIONS += transl/chinese.ts \
                transl/czech.ts \
                transl/english.ts \
                transl/russian.ts
