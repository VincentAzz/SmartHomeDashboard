<template>
    <div class="wrapper">
        <div class="body-wrapper">
            <div class="body">
                <div class="data-header"><span>消息日志</span></div>

                <div class="data-wrapper">
                    <div class="logsList">
                        <div class="data-text">
                            <view :v-for="logsList" class="data-value">                      
                                {{logsList}}
                            </view>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<!---------------------------------------------------------------->

<script>
export default {
    data() {
        return {
            logsList:[]

        };
    },

    components: {},

    methods: {

    },

    created() {
    },

    onShow() {
        const _this = this; 
    }
}
</script>

<!---------------------------------------------------------------->

<style lang="scss" scoped>
.wrapper {
    padding: 12px;
    background-color: #f1f2f6;
    .btn {
        margin-top: 16px;
        border-radius: 10px;
        border: 2px solid #505050;
        color: #000000;
        background-color: #ffffff;
    }

    .data-wrapper {
        margin-top: 8px;
        display: flex;
        justify-content: space-between;

}

    .logsList {
        background-color: #ffffff;
        width: 100%;
        height: 500px;
        border-radius: 20px;
        display: flex;
        // padding: 20px 20px 20px 20px;
        .data-text {
            margin: 15px;
            color: #202020;

            .data-value {
                font-size: 12px;
                // text-align: left;
            }
        }
    }
}
</style>
