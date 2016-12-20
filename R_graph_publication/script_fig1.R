#install.packages("Hmisc", dependencies=T)
library("Hmisc")

setwd("C:/Users/cd3rg/Desktop/SmD_paper/")

fig1Data <- read.csv("source_data/data_figure1.csv", header=TRUE)
fig1Data <- t(fig1Data)
fig1Data <- fig1Data[-1,]

fig1DataNew <- fig1Data

fig1DataNew$H30 <- as.numeric(fig1Data[,1])
fig1DataNew$H332 <- as.numeric(fig1Data[,2])
fig1Data.args <- c('1-15', '4-36', '25-39', '28-42', '31-45', '34-48', '37-51', '40-60', '49-63', '52-66', '55-69', '58-72', '61-75', '64-78', '67-81', '70-84', '73-87', '76-90', '79-93', '82-96', '85-99', '88-119', 'ARNP', 'SmB')

tiff("fig1Aleft.tif", width = 7.5, height = 4, units = 'in', res = 600)
x<-barplot(fig1DataNew$H30,main="H30",ylab="IL-2(pg/ml)",col="BLACK", xaxt="n")
labs <- fig1Data.args
text(cex=1,x=x+1, y=-50, labs, xpd=TRUE, srt=90,pos=2)
dev.off()

tiff("fig1Aright.tif", width = 7.5, height = 4, units = 'in', res = 600)
x<-barplot(fig1DataNew$H332,main="H332",ylab="IL-2(pg/ml)",col="BLACK", xaxt="n")
labs <- fig1Data.args
text(cex=1,x=x+1, y=-50, labs, xpd=TRUE, srt=90,pos=2)
dev.off()

fig2Data <- read.csv("source_data/fig2.csv")
fig2Data.6190AVE <- fig2Data[c(1:8),2]
fig2Data.6190STD <- fig2Data[c(1:8),3]
fig2Data.SmDAVE <- fig2Data[c(1,9:15),2]
fig2Data.SmDSTD <- fig2Data[c(1,9:15),3]
fig2Data.SmBAVE <- fig2Data[c(1,16:22),2]
fig2Data.SmBSTD <- fig2Data[c(1,16:22),3]
fig2Data.SmAAVE <- fig2Data[c(1,23:29),2]
fig2Data.SmASTD <- fig2Data[c(1,23:29),3]
fig2Data.SmANOAPCAVE <- fig2Data[c(1,30:36),2]
fig2Data.SmANOAPCSTD <- fig2Data[c(1,30:36),3]
fig2Data.SmDNOAPCAVE <- fig2Data[c(1,37:43),2]
fig2Data.SmDNOAPCSTD <- fig2Data[c(1,37:43),3]
fig2Data.SmBNOAPCAVE <- fig2Data[c(1,44:50),2]
fig2Data.SmBNOAPCSTD <- fig2Data[c(1,44:50),3]

fig2DataNew<-data.frame(x=c(1:8))
fig2DataNew$x<-c(0,1,2,3,4,5,6,7)
fig2DataNew$y<-fig2Data.6190AVE
fig2DataNew$yDEV<-fig2Data.6190STD
fig2DataNew$xLab<-c("0","3.125","6.25","12.5","25","50","100","200")
plot(fig2DataNew$x, fig2DataNew$y, xaxes=FALSE,type="b", lwd=2, pch = 19,col="black", xaxt="n", yaxs="i")
axis(1, at = c(0:7), labels=fig2DataNew$xLab)
#arrows(fig2DataNew$x, fig2DataNew$y-fig2DataNew$yDEV, fig2DataNew$x,fig2DataNew$y+fig2DataNew$yDEV, length=0.05, angle=90, code=3)
with(
  data=fig2DataNew,
  expr=errbar(x, y, y+yDEV,y-yDEV,add=T,type="b",pch=19,cap=.015))

for(i in c(1:8)){
  labs<- fig2DataNew$xLab[i]
  text(cex=1,x=x-10, y=-20, labs, xpd=TRUE, srt=90,pos=2,offset=(i-1)*20)
}

