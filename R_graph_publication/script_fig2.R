#install.packages("Hmisc", dependencies=T)
#library("Hmisc")
library("ggplot2")
setwd("/Users/chao/Desktop/fu_lab/SmD_paper/")

geomLineSize <- 1.5
geomDotSize <- 4
xTextSize <- 15
yTextSize <- 15
labSize <- 15

fig2Data <- read.csv("source_data/fig2A.csv", header=TRUE)
fig2Data$Antigen <- factor(fig2Data$Antigen, levels=rev(levels(fig2Data$Antigen)))

tiff("fig2Aleft.tif", width = 7.5, height = 4, units = 'in', res = 600)
# Simple line plot
# Change point shapes and line types by groups
ggplot(fig2Data, aes(x=dose, y=ave, shape = Antigen, group = Antigen,fill=Antigen))+ 
  scale_shape_manual(values=(19-nlevels(fig2Data$Antigen)):19) +
  geom_errorbar(aes(ymin=ave-std, ymax=ave+std), width=.1, 
                position=position_dodge(0)) +
  geom_line(size=geomLineSize) +
  geom_point(size=geomDotSize)+
  scale_x_log10()+
  labs(title="F140-9 Hybridoma",x="Dose (ug/ml)", y = "IL-2(pg/ml)")+
  expand_limits(y=0) + 
  scale_y_continuous(breaks=0:400*50) + 
  guides(fill=guide_legend(title="Antigen"))+
  theme_classic()+
  geom_vline(xintercept = 0,size=3)+
  theme(legend.justification=c(1,0),
      legend.position=c(0.25,0.4))+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
      axis.text.y = element_text(size=yTextSize),
      axis.title=element_text(size=labSize,face="bold"))
dev.off()

fig2BData <- read.csv("source_data/fig2B.csv", header=TRUE)
#fig2BData$Antigen <- factor(fig2BData$Antigen, levels=c("Ro60 1-146","Ro60 132-277","Ro60 263-406","Ro60 394-538"))

tiff("fig2B.tif", width = 7.5, height = 4, units = 'in', res = 600)
# Simple line plot
# Change point shapes and line types by groups
ggplot(fig2BData, aes(x=dose, y=ave, shape = Antigen, group = Antigen,fill=Antigen))+ 
  scale_shape_manual(values=(19-nlevels(fig2BData$Antigen)):19) +
  geom_errorbar(aes(ymin=ave-std, ymax=ave+std), width=.1, 
                position=position_dodge(0)) +
  geom_line(size=geomLineSize) +
  geom_point(size=geomDotSize)+
  scale_x_log10()+
  labs(title="F140-9 Hybridoma",x="Dose (ug/ml)", y = "IL-2(pg/ml)")+
  expand_limits(y=0) + 
  scale_y_continuous(breaks=0:400*50) + 
  guides(fill=guide_legend(title="Antigen"))+
  theme_classic()+
  geom_vline(xintercept = 0,size=3)+
  theme(legend.justification=c(1,0),
        legend.position=c(0.25,0.4))+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
        axis.text.y = element_text(size=yTextSize),
        axis.title=element_text(size=labSize,face="bold"))

dev.off()