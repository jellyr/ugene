include (external_tool_support.pri)

# Input
HEADERS += src/ETSProjectViewItemsContoller.h \
           src/ExternalToolManager.h \
           src/ExternalToolSupportL10N.h \
           src/ExternalToolSupportPlugin.h \
           src/ExternalToolSupportSettings.h \
           src/ExternalToolSupportSettingsController.h \
           src/RnaSeqCommon.h \
           src/TaskLocalStorage.h \
           src/bedtools/BedtoolsSupport.h \
           src/bedtools/BedtoolsSupportTask.h \
           src/blast/BlastAllSupport.h \
           src/blast/BlastAllSupportRunDialog.h \
           src/blast/BlastAllSupportTask.h \
           src/blast/BlastAllWorker.h \
           src/blast/FormatDBSupport.h \
           src/blast/FormatDBSupportRunDialog.h \
           src/blast/FormatDBSupportTask.h \
           src/blast_plus/BlastDBCmdDialog.h \
           src/blast_plus/BlastDBCmdSupport.h \
           src/blast_plus/BlastDBCmdSupportTask.h \
           src/blast_plus/BlastNPlusSupportTask.h \
           src/blast_plus/BlastPlusSupport.h \
           src/blast_plus/BlastPlusSupportCommonTask.h \
           src/blast_plus/BlastPlusSupportRunDialog.h \
           src/blast_plus/BlastPlusWorker.h \
           src/blast_plus/BlastPPlusSupportTask.h \
           src/blast_plus/BlastXPlusSupportTask.h \
           src/blast_plus/RPSBlastSupportTask.h \
           src/blast_plus/TBlastNPlusSupportTask.h \
           src/blast_plus/TBlastXPlusSupportTask.h \
           src/bowtie/bowtie_tests/bowtieTests.h \
           src/bowtie/BowtieSettingsWidget.h \
           src/bowtie/BowtieSupport.h \
           src/bowtie/BowtieTask.h \
           src/bowtie2/Bowtie2Support.h \
           src/bowtie2/Bowtie2Task.h \
           src/bowtie2/Bowtie2SettingsWidget.h \
           src/bwa/bwa_tests/bwaTests.h \
           src/bwa/BwaSettingsWidget.h \
           src/bwa/BwaSupport.h \
           src/bwa/BwaTask.h \
           src/bwa/BwaWorker.h \
           src/cap3/CAP3Support.h \
           src/cap3/CAP3SupportDialog.h \
           src/cap3/CAP3SupportTask.h \
           src/cap3/CAP3Worker.h \
           src/ceas/CEASReportWorker.h \
           src/ceas/CEASSettings.h \
           src/ceas/CEASSupport.h \
           src/ceas/CEASSupportTask.h \
           src/clustalo/ClustalOSupport.h \
           src/clustalo/ClustalOSupportRunDialog.h \
           src/clustalo/ClustalOSupportTask.h \
           src/clustalo/ClustalOWorker.h \
           src/clustalw/ClustalWSupport.h \
           src/clustalw/ClustalWSupportRunDialog.h \
           src/clustalw/ClustalWSupportTask.h \
           src/clustalw/ClustalWWorker.h \
           src/conduct_go/ConductGOSettings.h \
           src/conduct_go/ConductGOSupport.h \
           src/conduct_go/ConductGOTask.h \
           src/conduct_go/ConductGOWorker.h \
           src/conservation_plot/ConservationPlotSettings.h \
           src/conservation_plot/ConservationPlotSupport.h \
           src/conservation_plot/ConservationPlotTask.h \
           src/conservation_plot/ConservationPlotWorker.h \
           src/cufflinks/CuffdiffSupportTask.h \
           src/cufflinks/CuffdiffWorker.h \
           src/cufflinks/CufflinksSettings.h \
           src/cufflinks/CufflinksSupport.h \
           src/cufflinks/CufflinksSupportTask.h \
           src/cufflinks/CufflinksWorker.h \
           src/cufflinks/CuffmergeSupportTask.h \
           src/cufflinks/CuffmergeWorker.h \
           src/cufflinks/GffreadSupportTask.h \
           src/cufflinks/GffreadWorker.h \
           src/macs/MACSSettings.h \
           src/macs/MACSSupport.h \
           src/macs/MACSTask.h \
           src/macs/MACSWorker.h \
           src/mafft/MAFFTSupport.h \
           src/mafft/MAFFTSupportRunDialog.h \
           src/mafft/MAFFTSupportTask.h \
           src/mafft/MAFFTWorker.h \
           src/mrbayes/MrBayesDialogWidget.h \
           src/mrbayes/MrBayesSupport.h \
           src/mrbayes/MrBayesTask.h \
           src/mrbayes/MrBayesTests.h \
           src/peak2gene/Peak2GeneSettings.h \
           src/peak2gene/Peak2GeneSupport.h \
           src/peak2gene/Peak2GeneTask.h \
           src/peak2gene/Peak2GeneWorker.h \
           src/perl/PerlSupport.h \
           src/python/PythonSupport.h \
           src/R/RSupport.h \
           src/samtools/BcfToolsSupport.h \
           src/samtools/SamToolsExtToolSupport.h \
           src/samtools/TabixSupportTask.h \
           src/samtools/TabixSupport.h \
           src/seqpos/SeqPosSettings.h \
           src/seqpos/SeqPosSupport.h \
           src/seqpos/SeqPosTask.h \
           src/seqpos/SeqPosWorker.h \
           src/spidey/SpideySupport.h \
           src/spidey/SpideySupportTask.h \
           src/tcoffee/TCoffeeSupport.h \
           src/tcoffee/TCoffeeSupportRunDialog.h \
           src/tcoffee/TCoffeeSupportTask.h \
           src/tcoffee/TCoffeeWorker.h \
           src/tophat/TopHatSettings.h \
           src/tophat/TopHatSupport.h \
           src/tophat/TopHatSupportTask.h \
           src/tophat/TopHatWorker.h \
           src/utils/BlastRunCommonDialog.h \
           src/utils/BlastTaskSettings.h \
           src/utils/CistromeDelegate.h \
           src/utils/ExportTasks.h \
           src/utils/ExternalToolSearchTask.h \
           src/utils/ExternalToolSupportAction.h \
           src/utils/ExternalToolValidateTask.h \
           src/utils/ExternalToolUtils.h \
           src/vcftools/VcfConsensusWorker.h \
           src/vcftools/VcfConsensusSupport.h \
           src/vcftools/VcfConsensusSupportTask.h \
           src/vcfutils/VcfutilsSupport.h
FORMS += src/ui/BlastAllSupportDialog.ui \
         src/ui/BlastDBCmdDialog.ui \
         src/ui/BowtieBuildSettings.ui \
         src/ui/BowtieSettings.ui \
         src/ui/Bowtie2Settings.ui \
         src/ui/BwaBuildSettings.ui \
         src/ui/BwaSettings.ui \
         src/ui/BwaMemSettings.ui \
         src/ui/BwaSwSettings.ui \
         src/ui/CAP3SupportDialog.ui \
         src/ui/ClustalOSupportRunDialog.ui \
         src/ui/ClustalWSupportRunDialog.ui \
         src/ui/ETSSettingsWidget.ui \
         src/ui/FormatDBSupportRunDialog.ui \
         src/ui/MAFFTSupportRunDialog.ui \
         src/ui/MrBayesDialog.ui \
         src/ui/SelectPathDialog.ui \
         src/ui/TCoffeeSupportRunDialog.ui
SOURCES += src/ETSProjectViewItemsContoller.cpp \
           src/ExternalToolManager.cpp \
           src/ExternalToolSupportPlugin.cpp \
           src/ExternalToolSupportSettings.cpp \
           src/ExternalToolSupportSettingsController.cpp \
           src/RnaSeqCommon.cpp \
           src/TaskLocalStorage.cpp \
           src/bedtools/BedtoolsSupport.cpp \
           src/bedtools/BedtoolsSupportTask.cpp \
           src/blast/BlastAllSupport.cpp \
           src/blast/BlastAllSupportRunDialog.cpp \
           src/blast/BlastAllSupportTask.cpp \
           src/blast/BlastAllWorker.cpp \
           src/blast/FormatDBSupport.cpp \
           src/blast/FormatDBSupportRunDialog.cpp \
           src/blast/FormatDBSupportTask.cpp \
           src/blast_plus/BlastDBCmdDialog.cpp \
           src/blast_plus/BlastDBCmdSupport.cpp \
           src/blast_plus/BlastDBCmdSupportTask.cpp \
           src/blast_plus/BlastNPlusSupportTask.cpp \
           src/blast_plus/BlastPlusSupport.cpp \
           src/blast_plus/BlastPlusSupportCommonTask.cpp \
           src/blast_plus/BlastPlusSupportRunDialog.cpp \
           src/blast_plus/BlastPlusWorker.cpp \
           src/blast_plus/BlastPPlusSupportTask.cpp \
           src/blast_plus/BlastXPlusSupportTask.cpp \
           src/blast_plus/RPSBlastSupportTask.cpp \
           src/blast_plus/TBlastNPlusSupportTask.cpp \
           src/blast_plus/TBlastXPlusSupportTask.cpp \
           src/bowtie/bowtie_tests/bowtieTests.cpp \
           src/bowtie/BowtieSettingsWidget.cpp \
           src/bowtie/BowtieSupport.cpp \
           src/bowtie/BowtieTask.cpp \
           src/bowtie2/Bowtie2Support.cpp \
           src/bowtie2/Bowtie2Task.cpp \
           src/bowtie2/Bowtie2SettingsWidget.cpp \
           src/bwa/bwa_tests/bwaTests.cpp \
           src/bwa/BwaSettingsWidget.cpp \
           src/bwa/BwaSupport.cpp \
           src/bwa/BwaTask.cpp \
           src/bwa/BwaWorker.cpp \
           src/cap3/CAP3Support.cpp \
           src/cap3/CAP3SupportDialog.cpp \
           src/cap3/CAP3SupportTask.cpp \
           src/cap3/CAP3Worker.cpp \
           src/ceas/CEASReportWorker.cpp \
           src/ceas/CEASSettings.cpp \
           src/ceas/CEASSupport.cpp \
           src/ceas/CEASSupportTask.cpp \
           src/clustalo/ClustalOSupport.cpp \
           src/clustalo/ClustalOSupportRunDialog.cpp \
           src/clustalo/ClustalOSupportTask.cpp \
           src/clustalo/ClustalOWorker.cpp \
           src/clustalw/ClustalWSupport.cpp \
           src/clustalw/ClustalWSupportRunDialog.cpp \
           src/clustalw/ClustalWSupportTask.cpp \
           src/clustalw/ClustalWWorker.cpp \
           src/conduct_go/ConductGOSettings.cpp \
           src/conduct_go/ConductGOSupport.cpp \
           src/conduct_go/ConductGOTask.cpp \
           src/conduct_go/ConductGOWorker.cpp \
           src/conservation_plot/ConservationPlotSettings.cpp \
           src/conservation_plot/ConservationPlotSupport.cpp \
           src/conservation_plot/ConservationPlotTask.cpp \
           src/conservation_plot/ConservationPlotWorker.cpp \
           src/cufflinks/CuffdiffSupportTask.cpp \
           src/cufflinks/CuffdiffWorker.cpp \
           src/cufflinks/CufflinksSettings.cpp \
           src/cufflinks/CufflinksSupport.cpp \
           src/cufflinks/CufflinksSupportTask.cpp \
           src/cufflinks/CufflinksWorker.cpp \
           src/cufflinks/CuffmergeSupportTask.cpp \
           src/cufflinks/CuffmergeWorker.cpp \
           src/cufflinks/GffreadSupportTask.cpp \
           src/cufflinks/GffreadWorker.cpp \
           src/macs/MACSSettings.cpp \
           src/macs/MACSSupport.cpp \
           src/macs/MACSTask.cpp \
           src/macs/MACSWorker.cpp \
           src/mafft/MAFFTSupport.cpp \
           src/mafft/MAFFTSupportRunDialog.cpp \
           src/mafft/MAFFTSupportTask.cpp \
           src/mafft/MAFFTWorker.cpp \
           src/mrbayes/MrBayesDialogWidget.cpp \
           src/mrbayes/MrBayesSupport.cpp \
           src/mrbayes/MrBayesTask.cpp \
           src/mrbayes/MrBayesTests.cpp \
           src/peak2gene/Peak2GeneSettings.cpp \
           src/peak2gene/Peak2GeneSupport.cpp \
           src/peak2gene/Peak2GeneTask.cpp \
           src/peak2gene/Peak2GeneWorker.cpp \
           src/perl/PerlSupport.cpp \
           src/python/PythonSupport.cpp \
           src/R/RSupport.cpp \
           src/samtools/BcfToolsSupport.cpp \
           src/samtools/SamToolsExtToolSupport.cpp \
           src/samtools/TabixSupportTask.cpp \
           src/samtools/TabixSupport.cpp \
           src/seqpos/SeqPosSettings.cpp \
           src/seqpos/SeqPosSupport.cpp \
           src/seqpos/SeqPosTask.cpp \
           src/seqpos/SeqPosWorker.cpp \
           src/spidey/SpideySupport.cpp \
           src/spidey/SpideySupportTask.cpp \
           src/tcoffee/TCoffeeSupport.cpp \
           src/tcoffee/TCoffeeSupportRunDialog.cpp \
           src/tcoffee/TCoffeeSupportTask.cpp \
           src/tcoffee/TCoffeeWorker.cpp \
           src/tophat/TopHatSettings.cpp \
           src/tophat/TopHatSupport.cpp \
           src/tophat/TopHatSupportTask.cpp \
           src/tophat/TopHatWorker.cpp \
           src/utils/BlastRunCommonDialog.cpp \
           src/utils/BlastTaskSettings.cpp \
           src/utils/CistromeDelegate.cpp \
           src/utils/ExportTasks.cpp \
           src/utils/ExternalToolSearchTask.cpp \
           src/utils/ExternalToolSupportAction.cpp \
           src/utils/ExternalToolUtils.cpp \
           src/utils/ExternalToolValidateTask.cpp \
           src/vcftools/VcfConsensusWorker.cpp \
           src/vcftools/VcfConsensusSupport.cpp \
           src/vcftools/VcfConsensusSupportTask.cpp \
           src/vcfutils/VcfutilsSupport.cpp
RESOURCES += external_tool_support.qrc
TRANSLATIONS += transl/chinese.ts \
                transl/czech.ts \
                transl/english.ts \
                transl/russian.ts




