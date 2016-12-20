library("ggplot2")

#setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

fig6AData <- read.csv("source_data/fig6A.csv")

tiff("fig6A.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig6AData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD57-71")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()


fig6BData <- read.csv("source_data/fig6B.csv")

tiff("fig6B.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig6BData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center', 
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD91-119")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()

fig6CData <- read.csv("source_data/fig6C.csv")

tiff("fig6C.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig6CData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD66-80")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()

fig6DData <- read.csv("source_data/fig6D.csv")

tiff("fig6D.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig6DData,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center', 
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  geom_vline(xintercept = 0.5,size=1)+
  geom_hline(yintercept = 0,size=1)+
  labs(title="SmD78-92")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()