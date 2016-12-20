library("ggplot2")

setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

fig8Data <- read.csv("source_data/fig8.csv")

tiff("fig8.tif", width = 3.5, height = 4, units = 'in', res = 600)
ggplot(fig8Data,aes(x=Protein,y=OD))+
  geom_dotplot(binaxis="y",stackdir='center',
               stackratio=1,dotsize=0.8) +
  stat_summary(fun.y = mean, fun.ymin = mean, fun.ymax = mean,
               geom = "crossbar", width = 0.5)+
  theme_classic()+
  labs(title="SmD high titre")+
  theme(axis.text.x = element_text(size=15,angle = 45, hjust = 1),
        axis.text.y = element_text(size=15),
        axis.title=element_text(size=15,face="bold"))
dev.off()