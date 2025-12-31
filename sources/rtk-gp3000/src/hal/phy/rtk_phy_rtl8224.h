

extern int rtl8224_probe(struct phy_device *phydev);
extern int rtl8224_get_features(struct phy_device *phydev);
extern int rtl8224_config_init(struct phy_device *phydev);
extern int rtl8224_cable_test_start(struct phy_device *phydev);
extern int rtl8224_cable_test_get_status(struct phy_device *phydev, bool *finished);
extern int rtl8224_get_tunable(struct phy_device *phydev, struct ethtool_tunable *tuna, void *data);
extern int rtl8224_set_tunable(struct phy_device *phydev, struct ethtool_tunable *tuna, const void *data);
extern int rtl8224_config_intr(struct phy_device *phydev);
extern int rtl8224_ack_intr(struct phy_device *phydev);
extern irqreturn_t rtl8224_handle_intr(struct phy_device *phydev);

