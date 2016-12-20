library("ggplot2")

#setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

xTextSize <- 15
yTextSize <- 15
labSize <- 15


fig5AData <- read.csv("source_data/fig5A.csv", header=TRUE)
fig5AData$Antigen <- factor(fig5AData$Antigen, levels=rev(levels(fig5AData$Antigen)))

tiff("fig5A.tif", width = 7.5, height = 4, units = 'in', res = 600)
# Simple line plot
# Change point shapes and line types by groups
ggplot(fig5AData, aes(x=dose, y=ave))+ 
  scale_shape_manual(values=(19-nlevels(fig5AData$Antigen)):19) +
  geom_errorbar(aes(ymin=ave-std, ymax=ave+std), width=.1, 
                position=position_dodge(0)) +
  geom_line(size=geomLineSize) +
  geom_point(size=geomDotSize)+
  labs(title="F140-9 Hybridoma",x="Dose (ug/ml)", y = "IL-2(pg/ml)")+
  expand_limits(y=0) + 
  #scale_y_continuous(breaks=0:400*50) + 
  guides(fill=guide_legend(title="Antigen"))+
  theme_classic()+
  geom_vline(xintercept = 0,size=1)+
  geom_hline(yintercept = 0,size=1)+
  theme(legend.justification=c(1,0),
        legend.position=c(0.25,0.4))+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
        axis.text.y = element_text(size=yTextSize),
        axis.title=element_text(size=labSize,face="bold"))

dev.off()



fig5BData <- read.csv("source_data/fig5B.csv", header=TRUE)
fig5BData$Antigen <- factor(fig5BData$Antigen, levels=rev(levels(fig5BData$Antigen)))

tiff("fig5B.tif", width = 7.5, height = 4, units = 'in', res = 600)
# Simple line plot
# Change point shapes and line types by groups
ggplot(fig5BData, aes(x=dose, y=ave))+ 
  scale_shape_manual(values=(19-nlevels(fig5BData$Antigen)):19) +
  geom_errorbar(aes(ymin=ave-std, ymax=ave+std), width=.1, 
                position=position_dodge(0)) +
  geom_line(size=geomLineSize) +
  geom_point(size=geomDotSize)+
  labs(title="F140-9 Hybridoma",x="Dose (ug/ml)", y = "IL-2(pg/ml)")+
  expand_limits(y=0) + 
  scale_y_continuous(breaks=0:400*50) + 
  guides(fill=guide_legend(title="Antigen"))+
  theme_classic()+
  geom_vline(xintercept = 0,size=1)+
  geom_hline(yintercept = 0,size=1)+
  theme(legend.justification=c(1,0),
        legend.position=c(0.25,0.4))+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
        axis.text.y = element_text(size=yTextSize),
        axis.title=element_text(size=labSize,face="bold"))

dev.off()


fig5CData <- read.csv("source_data/fig5C.csv", header=TRUE)

# fig1DataNew$H30 <- as.numeric(fig1Data[,1])
# fig1DataNew$H332 <- as.numeric(fig1Data[,2])
# fig1Data.args <- c('1-15', '4-36', '25-39', '28-42', '31-45', '34-48', '37-51', '40-60', '49-63', '52-66', '55-69', '58-72', '61-75', '64-78', '67-81', '70-84', '73-87', '76-90', '79-93', '82-96', '85-99', '88-119', 'ARNP', 'SmB')


tiff("fig5C.tif", width = 7.5, height = 4, units = 'in', res = 600)
ggplot(fig5CData, aes(x=group, y=AVE)) + 
  geom_bar(stat="identity", position=position_dodge(),fill="black") +
  geom_errorbar(aes(ymin=AVE-SD, ymax=AVE+SD), width=.2,
                position=position_dodge(.9)) +
  xlab("")+
  ylab("IFN-r(pg/ml)")+
  theme_classic()+
  geom_vline(xintercept = 0,size=1)+
  geom_hline(yintercept = 0,size=1)+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
        axis.text.y = element_text(size=yTextSize),
        axis.title=element_text(size=labSize,face="bold"))
dev.off()