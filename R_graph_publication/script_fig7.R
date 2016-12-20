library("ggplot2")

#setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

fig7AData <- read.csv("source_data/fig7A.csv")

tiff("fig7A.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7AData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80.M7 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig7BData <- read.csv("source_data/fig7B.csv")

tiff("fig7B.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7BData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig7CData <- read.csv("source_data/fig7C.csv")

tiff("fig7C.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7CData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80.M2 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig7DData <- read.csv("source_data/fig7D.csv")

tiff("fig7D.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7DData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80.M11 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()

fig7EData <- read.csv("source_data/fig7E.csv")

tiff("fig7E.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7EData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80.M7 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig7FData <- read.csv("source_data/fig7F.csv")

tiff("fig7F.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7FData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80 immunization")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig7GData <- read.csv("source_data/fig7G.csv", header=TRUE)

tiff("fig7G.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7GData, aes(x=group, y=AVE)) + 
  geom_bar(stat="identity", position=position_dodge(),fill="black") +
  geom_errorbar(aes(ymin=AVE-SD, ymax=AVE+SD), width=.2,
                position=position_dodge(.9)) +
  xlab("")+
  ylab("relative unit")+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  theme(axis.text.x = element_text(size=10,angle = 45, hjust = 1),
        axis.text.y = element_text(size=10),
        axis.title=element_text(size=10,face="bold"))
dev.off()


fig7HData <- read.csv("source_data/fig7H.csv", header=TRUE)


tiff("fig7H.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig7HData, aes(x=group, y=AVE)) + 
  geom_bar(stat="identity", position=position_dodge(),fill="black") +
  geom_errorbar(aes(ymin=AVE-SD, ymax=AVE+SD), width=.2,
                position=position_dodge(.9)) +
  xlab("")+
  ylab("IL-2(pg/ml)")+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  theme(axis.text.x = element_text(size=10,angle = 45, hjust = 1),
        axis.text.y = element_text(size=10),
        axis.title=element_text(size=10,face="bold"))
dev.off()