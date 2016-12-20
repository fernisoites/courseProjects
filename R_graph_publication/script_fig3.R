#install.packages("Hmisc", dependencies=T)
library("ggplot2")

setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

xTextSize <- 15
yTextSize <- 15
labSize <- 15

fig3leftData <- read.csv("source_data/figure3Aleft.csv", header=TRUE)

# fig1DataNew$H30 <- as.numeric(fig1Data[,1])
# fig1DataNew$H332 <- as.numeric(fig1Data[,2])
# fig1Data.args <- c('1-15', '4-36', '25-39', '28-42', '31-45', '34-48', '37-51', '40-60', '49-63', '52-66', '55-69', '58-72', '61-75', '64-78', '67-81', '70-84', '73-87', '76-90', '79-93', '82-96', '85-99', '88-119', 'ARNP', 'SmB')


tiff("fig3Aleft.tif", width = 5, height = 4, units = 'in', res = 600)
ggplot(fig3leftData, aes(x=group, y=AVE)) + 
  geom_bar(stat="identity", position=position_dodge(),fill="black") +
  geom_errorbar(aes(ymin=AVE-SD, ymax=AVE+SD), width=.2,
                position=position_dodge(.9)) +
  xlab("")+
  ylab("IL-2(pg/ml)")+
  theme_classic()+
  
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
        axis.text.y = element_text(size=yTextSize),
        axis.title=element_text(size=labSize,face="bold"))
dev.off()



fig3Data <- read.csv("source_data/figure3Aright.csv", header=TRUE)

# fig1DataNew$H30 <- as.numeric(fig1Data[,1])
# fig1DataNew$H332 <- as.numeric(fig1Data[,2])
# fig1Data.args <- c('1-15', '4-36', '25-39', '28-42', '31-45', '34-48', '37-51', '40-60', '49-63', '52-66', '55-69', '58-72', '61-75', '64-78', '67-81', '70-84', '73-87', '76-90', '79-93', '82-96', '85-99', '88-119', 'ARNP', 'SmB')


tiff("fig3Aright.tif", width = 5, height = 4, units = 'in', res = 600)
ggplot(fig3Data, aes(x=group, y=AVE)) + 
  geom_bar(stat="identity", position=position_dodge(),fill="black") +
  geom_errorbar(aes(ymin=AVE-SD, ymax=AVE+SD), width=.2,
                position=position_dodge(.9)) +
  xlab("")+
  ylab("IL-2(pg/ml)")+
  coord_flip()+
  theme_classic()+
  scale_x_discrete(breaks=NULL)+
  theme(axis.text.x = element_text(size=xTextSize,angle = 45, hjust = 1),
      axis.text.y = element_text(size=yTextSize),
      axis.title=element_text(size=labSize,face="bold"))
dev.off()


