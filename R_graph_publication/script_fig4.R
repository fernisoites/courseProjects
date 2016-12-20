#install.packages("Hmisc", dependencies=T)
library("ggplot2")

geomLineSize <- 1.5
geomDotSize <- 4
xTextSize <- 15
yTextSize <- 15
labSize <- 15



fig4Data <- read.csv("source_data/fig4Bleft.csv", header=TRUE)
colnames(fig4Data)<-gsub(pattern = "SMD66.", replacement = "SmD66-", x = colnames(fig4Data), ignore.case = F)
fig4Data <- t(fig4Data)

tiff("fig4Bleft.tif", width = 7.5, height = 4, units = 'in', res = 600)
x<-barplot(fig4Data[,1],main="H19",ylab="IL-2(pg/ml)",col="BLACK", xaxt="n")
labs <- rownames(fig4Data)
text(cex=1,x=x+0.4, y=-5, labs, xpd=TRUE, srt=45,pos=2)
dev.off()



fig4rightData <- read.csv("source_data/fig4Bright.csv", header=TRUE)
fig4rightData$Antigen <- factor(fig4rightData$Antigen, levels=rev(levels(fig4rightData$Antigen)))

tiff("fig4Bright.tif", width = 7.5, height = 4, units = 'in', res = 600)
# Simple line plot
# Change point shapes and line types by groups
ggplot(fig4rightData, aes(x=dose, y=ave))+ 
  scale_shape_manual(values=(19-nlevels(fig4rightData$Antigen)):19) +
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
